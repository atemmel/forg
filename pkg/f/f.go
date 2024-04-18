package f

type mapFunc[From, To any] func(From) To

func Map[From, To any](from []From, f mapFunc[From, To]) []To {
	to := make([]To, len(from))
	for i, x := range from {
		to[i] = f(x)
	}
	return to
}
