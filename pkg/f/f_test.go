package f

import (
	"strconv"
	"testing"
)

func TestMap(t *testing.T) {
	nums := []int{0, 1, 2, 3, 4}

	strs := Map(nums, strconv.Itoa)

	if strs[0] != "0" {
		t.Fatal()
	}
	if strs[4] != "4" {
		t.Fatal()
	}
}
