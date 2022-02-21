package fstools

import (
	"strings"
)

func TrimSuffix(s, suffix string) string {
	if strings.HasSuffix(s, suffix) {
		s = s[:len(s)-len(suffix)]
	}
	return s
}
func CSVtoArray(tagcsv string) []string {
	var tagcsv_array []string
	if strings.Contains(tagcsv, ",") {
		return strings.Split(tagcsv, ",")
	} else if len(tagcsv) > 0 {
		tagcsv_array = make([]string, 1)
		tagcsv_array[0] = tagcsv
	} else {
		tagcsv_array = make([]string, 0)
	}
	return tagcsv_array
}
