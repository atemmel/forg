package compile

import (
	"fmt"
	"math/rand"
	"os"
	"path"
	"testing"
)

const (
	cppPreamble = `
#include <algorithm>
#include <any>
#include <array>
#include <bitset>
#include <chrono>
#include <cmath>
#include <deque>
#include <forward_list>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
`
)

func randomStr(length, seed int64) string {
	const letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
	src := rand.NewSource(seed)
	b := make([]byte, length)
    for i := range b {
        b[i] = letters[src.Int63() % int64(len(letters))]
    }
    return string(b)

}

func writeFile(dir, file, content string) string {
	fullpath := path.Join(dir, file)
	err := os.WriteFile(fullpath, []byte(content), 0o644)
	if err != nil {
		panic(err)
	}
	return fullpath
}

func createSrc(i int64) (string, string, string) {
	name := randomStr(16, i)

	hpp := fmt.Sprintf(`#pragma once

void %s();`, name)
	
	cpp := fmt.Sprintf(`
%s

void %s() {
	std::cout << "%s\n";
}
	`, cppPreamble, name, name)

	return name, hpp, cpp
}

func setup(t *testing.T) *Target {
	dir := t.TempDir()

	const n = 64

	target := &Target{
		Units: make([]Unit, n),
	}

	for i := int64(0); i < n; i++ {
		name, hppSrc, cppSrc := createSrc(i)
		hppPath := fmt.Sprintf("%s.hpp", name)
		cppPath := fmt.Sprintf("%s.cpp", name)

		_ = writeFile(dir, hppPath, hppSrc)
		fullpath := writeFile(dir, cppPath, cppSrc)

		target.Units[i] = Unit{
			Path: fullpath,
		}
	}

	return target
}

func TestCompile(t *testing.T) {
	target := setup(t)

	err := Compile(target)
	if err != nil {
		t.Fatal("Error on compilation:", err)
	}
}

func TestCompileWithError(t *testing.T) {
	target := setup(t)

	l := len(target.Units)
	half := l / 2

	target.Units = append(target.Units[:half + 1], target.Units[half:]...)
	target.Units[half] = Unit{
		Path: "xyz.cpp",
	}

	err := Compile(target)
	if err == nil {
		t.Fatal("Compilation suceeded unexpectedly")
	}
}
