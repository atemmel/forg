package fetch

import (
	"archive/zip"
	"bytes"
	"fmt"
	"forg/pkg/log"
	"forg/pkg/util"
	"io"
	"net/http"
	"os"
	"path"
	"path/filepath"
	"strings"
)

type Platform struct {
	Linux   string
	Windows string
}

type Opts struct {
	Filename           string // e.g the 'libraylib' in libraylib.a or libraylib.so
	Target             string // e.g 'x86_64-windows', blank means native
	ArchiveIncludePath string
	ArchiveLibPath     string
	UrlPrefix          string
	UrlFilename        Platform
	ArchiveKind        Platform // e.g '.tar.gz', '.zip', '.h' if single file
}

var (
	// as in the paths are static, not necessarily the lib
	StaticLibPaths = []string{
		"/lib",
		"/usr/local/lib",
	}

	linuxLibExtensions = []string{
		"so",
		"a",
	}

	windowsLibExtensions = []string{
		"a",
	}
)

func GetExtensionsFromTarget(target string) []string {
	target = util.CleanTarget(target)

	var extensions []string
	switch target {
	case "linux":
		extensions = linuxLibExtensions
	case "windows":
		extensions = windowsLibExtensions
	}
	log.Assert(len(extensions) != 0)
	return extensions
}

func GetLibDirFromCleanTarget(target string) string {
	return fmt.Sprintf("%s/%s/lib", GetForgSpecificPaths(), util.CleanTarget(target))
}

func GetIncludeDirFromCleanTarget(target string) string {
	return fmt.Sprintf("%s/%s/include", GetForgSpecificPaths(), util.CleanTarget(target))
}

func GetForgSpecificPaths() string {
	return path.Join(os.Getenv("HOME"), ".local/share/forg")
}

func makeLibIncludePathsIfNotAlreadyDone() {
	forgRoot := GetForgSpecificPaths()
	for _, target := range []string{"windows", "linux"} {
		log.AssertErr(os.MkdirAll(path.Join(forgRoot, target, "lib"), 0o755))
		log.AssertErr(os.MkdirAll(path.Join(forgRoot, target, "include"), 0o755))
	}
}

func FetchLibIfNotLocallyResolved(opts Opts) error {
	target := util.CleanTarget(opts.Target)

	if libExists(opts.Filename, target) {
		return nil
	}

	makeLibIncludePathsIfNotAlreadyDone()

	urlFilename := ""
	archiveKind := ""
	switch target {
	case "linux":
		urlFilename = opts.UrlFilename.Linux
		archiveKind = opts.ArchiveKind.Linux
	case "windows":
		urlFilename = opts.UrlFilename.Windows
		archiveKind = opts.ArchiveKind.Windows
	}
	log.Assert(urlFilename != "")
	log.Assert(archiveKind != "")

	fullurl := fmt.Sprintf("%s/%s%s", opts.UrlPrefix, urlFilename, archiveKind)

	resp, err := http.Get(fullurl)
	log.AssertErr(err)
	defer resp.Body.Close()

	switch archiveKind {
	case ".zip":
		extractZip(resp, urlFilename, target)
	case ".h": //TODO: this case is not fully complete
		writeFile(resp, "include", urlFilename+archiveKind, target)

	}

	return nil
}

func extractZip(resp *http.Response, urlFilename, target string) {
	zipBytes, err := io.ReadAll(resp.Body)
	log.AssertErr(err)
	err = resp.Body.Close()
	log.AssertErr(err)

	zipReader, err := zip.NewReader(bytes.NewReader(zipBytes), int64(len(zipBytes)))
	log.AssertErr(err)

	for _, file := range zipReader.File {
		if file.FileInfo().IsDir() {
			continue
		}

		for _, dir := range []string{"lib", "include"} {
			if strings.HasPrefix(file.Name, urlFilename+"/"+dir) {
				fileReader, err := file.Open()
				log.AssertErr(err)
				fileWriter, err := os.OpenFile(fmt.Sprintf("%s/%s/%s/%s", GetForgSpecificPaths(), target, dir, filepath.Base(file.Name)), os.O_CREATE|os.O_WRONLY, 0o644)
				log.AssertErr(err)
				_, err = io.Copy(fileWriter, fileReader)
				log.AssertErr(err)
			}
		}
	}
}

func writeFile(resp *http.Response, includeOrLib, filename, target string) {
	fileReader := resp.Body
	fileWriter, err := os.OpenFile(fmt.Sprintf("%s/%s/%s/%s", GetForgSpecificPaths(), target, includeOrLib, filename), os.O_CREATE|os.O_WRONLY, 0o644)
	log.AssertErr(err)
	_, err = io.Copy(fileWriter, fileReader)
	log.AssertErr(err)
}

func FetchHeaderIfNotLocallyResolved() {

}

func libExists(filename, target string) bool {
	var okExtensions []string
	switch target {
	case "linux":
		okExtensions = linuxLibExtensions
	case "windows":
		okExtensions = windowsLibExtensions
	}
	log.Assert(len(okExtensions) != 0)

	for _, dir := range StaticLibPaths {
		for _, ext := range okExtensions {
			fullpath := fmt.Sprintf("%s/%s.%s", dir, filename, ext)
			if util.Exists(fullpath) {
				return true
			}
		}
	}
	return false
}
