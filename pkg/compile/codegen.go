package compile

import (
	"forg/pkg/log"
	"os"
	"path"
	"strings"
	"time"
)

type Resources struct {
	Sprites []string
}

func findResources(o *Opts) Resources {
	r := Resources{
		Sprites: []string{},
	}

	f := path.Join(o.ProjectDir, "resources")
	sprites, err := recurseFind(f, ".png")
	if err != nil || len(sprites) == 0 {
		log.Stderr("WARN: no .png files found inside %s\n", f)
		return r
	}

	r.Sprites = sprites
	return r
}

func CodegenResources(o *Opts) error {
	r := findResources(o)
	_ = r
	fIncDir := path.Join(o.ProjectDir, "include", "generated")
	fSrcDir := path.Join(o.ProjectDir, "src", "generated")
	fInc := path.Join(fIncDir, "resources.hpp")
	fSrc := path.Join(fSrcDir, "resources.cpp")
	os.MkdirAll(fIncDir, 0o755)
	os.MkdirAll(fSrcDir, 0o755)

	err := codegenResourceInclude(o, r, fInc)
	if err != nil {
		return err
	}
	return codegenResourceSrc(o, r, fSrc)
}

func codegenResourceInclude(o *Opts, r Resources, f string) error {
	b := strings.Builder{}
	b.WriteString("#pragma once\n")
	b.WriteString("// Generated at " + time.Now().Format(time.RFC822Z))
	b.WriteString(`
#include <raylib.h>

auto initTextures() -> void;

auto deinitTextures() -> void;`)

	for _, s := range r.Sprites {
		word := funcName(s)
		b.WriteString("\n\nauto get")
		b.WriteString(word)
		b.WriteString("Texture() -> Texture;")
	}

	return os.WriteFile(f, []byte(b.String()), 0o755)
}

func codegenResourceSrc(o *Opts, r Resources, f string) error {
	b := strings.Builder{}
	b.WriteString("// Generated at ")
	b.WriteString(time.Now().Format(time.RFC822Z))
	b.WriteString("\n\n")
	b.WriteString(`#include <generated/resources.hpp>

#include <raylib.h>

`)

	for _, s := range r.Sprites {
		b.WriteString("Texture ")
		b.WriteString(funcName(s))
		b.WriteString(";\n")
	}

	b.WriteString(`
auto initTextures() -> void {
`)
	for _, s := range r.Sprites {
		fn := funcName(s)
		p := s[len(o.ProjectDir)+1:]
		b.WriteString("    ")
		b.WriteString(fn)
		b.WriteString(" = ")
		b.WriteString("LoadTexture(\"")
		b.WriteString(p)
		b.WriteString("\");\n")
	}
	b.WriteString("}")

	b.WriteString(`

auto deinitTextures() -> void {
`)
	for _, s := range r.Sprites {
		fn := funcName(s)
		b.WriteString("    ")
		b.WriteString("UnloadTexture(")
		b.WriteString(fn)
		b.WriteString(");\n")
	}
	b.WriteString("}")

	for _, s := range r.Sprites {
		word := funcName(s)
		b.WriteString("\n\nauto get")
		b.WriteString(word)
		b.WriteString("Texture() -> Texture {\n    return ")
		b.WriteString(word)
		b.WriteString(";\n}")
	}

	b.WriteByte('\n')

	return os.WriteFile(f, []byte(b.String()), 0o755)
}

func funcName(filename string) string {
	s := strip(path.Base(filename))
	s = s[:len(s)-3]
	words := strings.Split(s, " ")
	for i := 0; i < len(words); i++ {
		words[i] = strings.ToUpper(words[i][:1]) + words[i][1:]
	}
	return strings.Join(words, "")
}

func strip(s string) string {
	var result strings.Builder
	result.Grow(len(s))
	for i := 0; i < len(s); i++ {
		b := s[i]
		if ('a' <= b && b <= 'z') ||
			('A' <= b && b <= 'Z') ||
			('0' <= b && b <= '9') ||
			b == ' ' {
			result.WriteByte(b)
		} else if b == '-' || b == '_' {
			result.WriteByte(' ')
		}
	}
	return result.String()
}
