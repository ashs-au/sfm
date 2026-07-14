#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>

/* note v. 4 - compiled against libsndfile universal:
* sudo port uninstall libsndfile
* sudo port install libsndfile +universal

added default output format (list of seconds to suit ChucK snfbuf)
*/

// default (no flag) just dumps out list of secs
void print_usage(const char *prog_name) {
    printf("Usage: %s [options] <audiofile>\n", prog_name);
    printf("Options:\n");
    printf("  -c    Output as Csound score GEN23 text format (seconds)\n");
    printf("  -m    Output as Max/MSP 'coll' format (milliseconds)\n");
    printf("  -s    Output raw seconds (one per line)\n");
    printf("  -f    Output raw sample frames (one per line)\n");
    printf("  empty Output just milliseconds\n");
}

// 1. Comparison function for qsort (sorts by sample_offset ascending)
int compare_cues(const void *a, const void *b) {
	const SF_CUE_POINT *cueA = (const SF_CUE_POINT *)a;
	const SF_CUE_POINT *cueB = (const SF_CUE_POINT *)b;
	
	if (cueA->sample_offset < cueB->sample_offset) return -1;
	if (cueA->sample_offset > cueB->sample_offset) return 1;
	return 0;
}
    
int main(int argc, char *argv[]) {

	// allow the option - format_flag to be empty
    char *format_flag = "default";
    char *filename = NULL;

	// replaced arg checking in case of default format arg
	if (argc == 2) {
        format_flag = "default"; 
        filename = argv[1]; // Shifted: the filename is actually in argv
    } else if (argc >= 3) {
        format_flag = argv[1];
        filename = argv[2];
    } else {
        print_usage(argv[0]);
        return 1;
    }
	
	
    SF_INFO sfinfo;
    // Initialize sfinfo to 0 as required by libsndfile
    memset(&sfinfo, 0, sizeof(sfinfo)); 
    SNDFILE *infile = sf_open(filename, SFM_READ, &sfinfo);

    if (!infile) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        fprintf(stderr, "%s\n", sf_strerror(NULL));
        return 1;
    }

    /* -------------------------------------------------------------
     * A) EXTRACT & PRINT SOUND FILE INFO
     * ------------------------------------------------------------- */
    char *container = "Unknown";
    char *bit_depth = "Unknown";

    // Determine Container Format
    switch (sfinfo.format & SF_FORMAT_TYPEMASK) {
        case SF_FORMAT_WAV:  container = "WAV";  break;
        case SF_FORMAT_AIFF: container = "AIFF"; break;
        case SF_FORMAT_FLAC: container = "FLAC"; break;
        case SF_FORMAT_CAF:  container = "CAF";  break;
    }

    // Determine Bit Depth / Subtype
    switch (sfinfo.format & SF_FORMAT_SUBMASK) {
        case SF_FORMAT_PCM_S8: bit_depth = "8-bit PCM";  break;
        case SF_FORMAT_PCM_16: bit_depth = "16-bit PCM"; break;
        case SF_FORMAT_PCM_24: bit_depth = "24-bit PCM"; break;
        case SF_FORMAT_PCM_32: bit_depth = "32-bit PCM"; break;
        case SF_FORMAT_FLOAT:  bit_depth = "32-bit Float"; break;
        case SF_FORMAT_DOUBLE: bit_depth = "64-bit Float"; break;
    }

    double total_seconds = (double)sfinfo.frames / sfinfo.samplerate;

    printf("=========================================\n");
    printf("FILE INFO:    %s\n", filename);
    printf("Format:       %s (%s)\n", container, bit_depth);
    printf("Channels:     %d\n", sfinfo.channels);
    printf("Sample Rate:  %d Hz\n", sfinfo.samplerate);
    printf("Total Frames: %lld samples\n", (long long)sfinfo.frames);
    printf("Duration:     %.4f seconds\n", total_seconds);
    printf("=========================================\n\n");

	/* -------------------------------------------------------------
     * B) EXTRACT, SORT & PRINT MARKERS
     * ------------------------------------------------------------- */
    SF_CUES cues;
    int has_cues = sf_command(infile, SFC_GET_CUE, &cues, sizeof(cues));

    if (has_cues == 0 || cues.cue_count == 0) {
        fprintf(stderr, "No markers/cues found.\n");
        sf_close(infile);
        return 0;
    }

    // 2. Sort the array in place before doing any math or printing
    qsort(cues.cue_points, cues.cue_count, sizeof(SF_CUE_POINT), compare_cues);

    // 3. Print the re-ordered list
    for (int i = 0; i < cues.cue_count; i++) {
        double time_in_seconds = (double)cues.cue_points[i].sample_offset / sfinfo.samplerate;
        double time_in_ms = time_in_seconds * 1000.0;
        int sample_position = cues.cue_points[i].sample_offset;
        char *name = cues.cue_points[i].name;

        if (strcmp(format_flag, "-c") == 0) {
            printf("%.4f ", time_in_seconds);
        } else if (strcmp(format_flag, "-m") == 0) {
            // Note: 'i' here is now the chronological timeline index!
            printf("%d, %.2f; /* %s */\n", i, time_in_ms, name[0] ? name : "marker");
        } else if (strcmp(format_flag, "-s") == 0) {
            printf("%.4f\n", time_in_seconds);
        } else if (strcmp(format_flag, "-f") == 0) {
            printf("%d\n", sample_position);
        } else if (strcmp(format_flag, "default") == 0) {
        	// default output
        	printf("%.2f; \n", time_in_ms);
        }
    }

    if (strcmp(format_flag, "-c") == 0) {
        printf("\n");
    }

    sf_close(infile);
    return 0;
}