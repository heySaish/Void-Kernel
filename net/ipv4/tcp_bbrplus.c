/* BBRplus (Community Maintained) TCP Congestion Control
 *
 * BBRplus is a community-tuned variant of BBR designed for high-throughput,
 * ultra-low latency, and aggressive packet-loss resilience over Wi-Fi/4G/5G networks.
 */
#include <linux/module.h>
#include <net/tcp.h>
#include <linux/win_minmax.h>

#define BW_SCALE 24
#define BW_UNIT (1 << BW_SCALE)

#define BBRPLUS_SCALE 256
#define BBRPLUS_UNIT BBRPLUS_SCALE

static const int bbrplus_bw_rtts = 10;

/* Dynamic Pacing Gain cycles for BBRplus (8 phases) */
static const int bbrplus_pacing_gain[] = {
	BBRPLUS_UNIT * 5 / 4,
	BBRPLUS_UNIT * 3 / 4,
	BBRPLUS_UNIT,
	BBRPLUS_UNIT,
	BBRPLUS_UNIT,
	BBRPLUS_UNIT,
	BBRPLUS_UNIT,
	BBRPLUS_UNIT,
};

enum bbrplus_mode {
	BBRPLUS_STARTUP,
	BBRPLUS_DRAIN,
	BBRPLUS_PROBE_BW,
	BBRPLUS_PROBE_RTT,
};

struct bbrplus {
	u32	min_rtt_us;
	u32	min_rtt_stamp;
	struct minmax bw;
	u32	rtt_cnt;
	u32	next_rtt_delivered;
	u32	prev_ca_state:3,
		packet_conservation:1,
		restore_cwnd:1,
		round_start:1,
		tso_segs_goal:7,
		idle_restart:1,
		probe_rtt_round_done:1,
		full_bw_reached:1,
		full_bw_cnt:2,
		cycle_idx:3,
		has_seen_rtt:1,
		mode:2,
		unused:9;
	u32	prior_cwnd;
	u32	full_bw;
	u32	pacing_gain;
	u32	cwnd_gain;
};

static u64 bbrplus_bw(const struct sock *sk)
{
	struct bbrplus *bbr = inet_csk_ca(sk);
	return minmax_get(&bbr->bw);
}

static void bbrplus_init(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);
	struct bbrplus *bbr = inet_csk_ca(sk);

	bbr->min_rtt_us = minmax_get(&tp->rtt_min);
	bbr->min_rtt_stamp = tcp_time_stamp;
	bbr->mode = BBRPLUS_STARTUP;
	bbr->prev_ca_state = TCP_CA_Open;
	bbr->cycle_idx = 0;
	bbr->full_bw_reached = 0;
	bbr->full_bw_cnt = 0;
	bbr->full_bw = 0;
	bbr->prior_cwnd = 0;
	bbr->pacing_gain = BBRPLUS_UNIT * 2885 / 1000;
	bbr->cwnd_gain = BBRPLUS_UNIT * 2;
	minmax_reset(&bbr->bw, bbr->rtt_cnt, 0);

	cmpxchg(&sk->sk_pacing_status, SK_PACING_NONE, SK_PACING_NEEDED);
}

static void bbrplus_set_pacing_rate(struct sock *sk, u64 bw, int gain)
{
	struct tcp_sock *tp = tcp_sk(sk);
	u64 rate = bw * gain;

	rate >>= 8;
	rate = min_t(u64, rate, sk->sk_max_pacing_rate);
	if (tp->srtt_us)
		sk->sk_pacing_rate = max_t(u64, rate, 1);
}

static void bbrplus_set_cwnd(struct sock *sk, const struct rate_sample *rs,
			     u32 acked, u32 bw, int gain)
{
	struct tcp_sock *tp = tcp_sk(sk);
	struct bbrplus *bbr = inet_csk_ca(sk);
	u64 cwnd = 0;

	if (!acked)
		return;

	cwnd = (u64)bw * bbr->cwnd_gain;
	cwnd >>= 8;
	cwnd += tp->mss_cache * 4;

	if (bbr->mode == BBRPLUS_PROBE_BW)
		cwnd = max_t(u64, cwnd, bbr->prior_cwnd);

	tp->snd_cwnd = min_t(u32, cwnd, tp->snd_cwnd_clamp);
}

static void bbrplus_main(struct sock *sk, const struct rate_sample *rs)
{
	struct bbrplus *bbr = inet_csk_ca(sk);
	u64 bw;

	if (rs->delivered < 0 || rs->interval_us <= 0)
		return;

	bw = (u64)rs->delivered * BW_UNIT;
	do_div(bw, rs->interval_us);

	if (bw > minmax_get(&bbr->bw))
		minmax_running_max(&bbr->bw, bbrplus_bw_rtts, bbr->rtt_cnt, bw);

	bbrplus_set_pacing_rate(sk, bbrplus_bw(sk), bbr->pacing_gain);
	bbrplus_set_cwnd(sk, rs, rs->acked_sacked, (u32)bbrplus_bw(sk), bbr->cwnd_gain);
}

static u32 bbrplus_sndbuf_expand(struct sock *sk)
{
	return 3;
}

static u32 bbrplus_undo_cwnd(struct sock *sk)
{
	struct bbrplus *bbr = inet_csk_ca(sk);
	return max(tcp_sk(sk)->snd_cwnd, bbr->prior_cwnd);
}

static u32 bbrplus_ssthresh(struct sock *sk)
{
	return max(tcp_sk(sk)->snd_cwnd >> 1, 4U);
}

static void bbrplus_set_state(struct sock *sk, u8 new_state)
{
	struct bbrplus *bbr = inet_csk_ca(sk);
	bbr->prev_ca_state = new_state;
}

static struct tcp_congestion_ops tcp_bbrplus_cong_ops __read_mostly = {
	.flags		= TCP_CONG_NON_RESTRICTED,
	.name		= "bbrplus",
	.owner		= THIS_MODULE,
	.init		= bbrplus_init,
	.cong_control	= bbrplus_main,
	.sndbuf_expand	= bbrplus_sndbuf_expand,
	.undo_cwnd	= bbrplus_undo_cwnd,
	.ssthresh	= bbrplus_ssthresh,
	.set_state	= bbrplus_set_state,
};

static int __init bbrplus_register(void)
{
	BUILD_BUG_ON(sizeof(struct bbrplus) > ICSK_CA_PRIV_SIZE);
	return tcp_register_congestion_control(&tcp_bbrplus_cong_ops);
}

static void __exit bbrplus_unregister(void)
{
	tcp_unregister_congestion_control(&tcp_bbrplus_cong_ops);
}

module_init(bbrplus_register);
module_exit(bbrplus_unregister);

MODULE_AUTHOR("Saish <saishnotfound@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("BBRplus TCP Congestion Control (Community Maintained)");
