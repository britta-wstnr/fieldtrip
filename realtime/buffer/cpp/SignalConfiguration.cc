#include <SignalConfiguration.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool ChannelSelection::parseString(int len, const char *str) {
	int pos = 0;
	index.clear();
	label.clear();
	
	while (pos < len) {
		int idx;
		int start;
		
		// skip white space
		while (isspace(str[pos])) {
			if (++pos == len) return true;
		}
		
		// next character should be 0-9
		if (!isdigit(str[pos])) return false;
		idx = str[pos] - '0';
		if (++pos == len) return false;
		
		while (isdigit(str[pos])) {
			idx = 10*idx + (str[pos] - '0');
			if (++pos == len) return false;
		}
		
		if (idx==0) return false;
		
		// next character should be =
		if (str[pos] != '=') return false;
		if (++pos == len) return false;
				
		if (str[pos] == '"') {
			start = ++pos;
			
			// search next white space
			while (pos < len && str[pos]!='"') pos++;
			// check for empty "" or unterminated "....
			if (pos == len || pos == start) return false;
			
			// got a label "like this"
			index.push_back(idx-1);
			label.push_back(std::string(str + start, pos-start));
			pos++;
		} else {
			// mark label start
			start = pos;
		
			// search next white space
			while (!isspace(str[pos]) && pos < len) pos++;
			if (start == pos) return false;
			
			// got a plain label
			index.push_back(idx-1);
			label.push_back(std::string(str + start, pos-start));
		}
	} 
	return true;
}


int SignalConfiguration::parseFile(const char *filename) {
	FILE *fp;
	char line[2048];
	int lineCount = 0;
	int errorCount = 0;
	int addSave = 1, addStream = 1;

	chanSelSave.clear();
	chanSelStream.clear();
	maxChanSave = maxChanStream = 0;

	fp = fopen(filename, "r");
	if (fp==NULL) return -1;
	
	while (!feof(fp)) {
		if (fgets(line, sizeof(line), fp) == NULL) break;
		lineCount++;
	
		char *lp = line;
		while (isspace(*lp)) lp++;
		// ignore comments starting by ; or # as well as empty lines
		if (*lp == ';' || *lp == '#' || *lp==0) continue;
		if (strchr(lp, '=')) {
			int n,chn; 
			char lab[100];
			n = sscanf(lp, "%i=%99s", &chn, lab);
			if (n==2 && chn>0) {
				if (addSave) {
					if (chn>maxChanSave) maxChanSave = chn;
					chanSelSave.add(chn-1, lab);
				}
				if (addStream) {
					if (chn>maxChanStream) maxChanStream = chn;
					chanSelStream.add(chn-1, lab);
				}
				continue;
			}
		}
		if (!strncasecmp(lp, "downsample", 10)) {
			int ds = atoi(lp+10);
			if (ds>=1) {
				downSample=ds;
				continue;
			}
		}
		if (!strncasecmp(lp, "bworder", 7)) {
			int bwOrder = atoi(lp+7);
			if (bwOrder>=0) {
				order = bwOrder;
				continue;
			}
		}	
		if (!strncasecmp(lp, "bandwidth", 9)) {
			char *endptr;
			double bw;
			bw = strtod(lp+9, &endptr);
			if (endptr != lp+9) {
				bandwidth = bw;
				continue;
			}
		}
			if (!strncasecmp(lp, "batteryrefresh", 14)) {
			int br = atoi(lp+14);
			if (br>=0) {
				batteryRefresh = br;
				continue;
			}
		}	
		if (!strncasecmp(lp, "statusrefresh", 13)) {
			int sr = atoi(lp+13);
			if (sr>=0) {
				statusRefresh = sr;
				continue;
			}
		}
		if (!strncasecmp(lp, "splittrigger", 12)) {
			char nameA[256], nameB[256];
			int n;
			n = sscanf(lp+13, "%255s%255s", nameA, nameB);
			if (n==2) {
				splitTrigger = true;
				lowTriggerName  = nameA;
				highTriggerName = nameB;
				continue;
			}
		}	
		if (!strncasecmp(lp, "[select]", 8)) {
			addStream = addSave = 1;
			continue;
		}
		if (!strncasecmp(lp, "[stream]", 8)) {
			addStream = 1;
			addSave = 0;
			continue;
		}
		if (!strncasecmp(lp, "[save]", 6)) {
			addSave = 1;
			addStream = 0;
			continue;
		}
		fprintf(stderr, "Could not parse line %i:\n%s\n", lineCount, line);
		errorCount++;
	}
	fclose(fp);
	return errorCount;
}
