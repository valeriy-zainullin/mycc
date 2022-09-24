//return 1 * 2 * 3;
return 
	1 * 2u * 3U * 4L * 5LL * 6ULL + 2 * 3u * 4U * 5L * 6LL * 7ULL +
	20 * 30u * 40U * 50L * 60LL * 70ULL << 1 + 2 + 3
	<=
	1 * 20u * 30U * 4L * 5LL * 6ULL + 2 * 3u * 4U * 5L * 6LL * 7ULL +
	200 * 300000u * 4000000U * 50L * 60LL * 70ULL << 1 + 2 + 3
	<
	100000 * 20u * 30U * 4L * 5LL * 6ULL + 2 * 3u * 4U * 5L * 6LL * 7ULL +
	200 * 300000u * 4000000U * 50L * 60LL * 70ULL << 1 + 2 + 3
	;