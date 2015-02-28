/*
chunfeng yuan create library on 2014-10-30
 */

 // Return the number of cpu. If detection fails, it will return -1;
int get_nb_cpus()
{
		// Optmization for windows: use GetSystemInfo()
        char * s, * pos;
        FILE * f;
        int number = -1;

		// Reading /proc/cpuinfo is more reliable on current CPUs,
		// so put it first and try the old method if this one fails
        f = fopen("/proc/cpuinfo", "r");
        if (f != NULL) {
				s = (char *)calloc(1, 81);
				if (s != NULL) {
					// Get the latest value of "processor" element
					// and increment it by 1 and it that value
					// will be the number of CPU.
					number = -2;
					while (fgets(s, 80, f) != NULL) {
							pos = strstr(s, "processor");
							if (pos == s) {
									pos = strchr(s, ':');
									number = atoi(pos + 1);
							}
					}
					++number;
					free(s);
				}
				fclose(f);
        }

        #ifdef _SC_NPROCESSORS_ONLN
        // Try the usual method if _SC_NPROCESSORS_ONLN exist
        if (number == -1) {

			number   = sysconf(_SC_NPROCESSORS_ONLN);
			/* Fails on some archs */
			if (number < 1) {
				number = -1;
			}
        }
        #endif

        return number;
}

