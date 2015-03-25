#include "crivello_primi.hpp"
#include "nprimi_al.cpp"

void print_double_percentage (lint*, lint*, const char*, lint*, lint*, const char*, lint, lint, char);
void print_percentage (lint*, lint*, lint, const char*);
void print_infos ();
void welcome ();
void print_license ();
void print_version ();
void print_param_err ();
void write_to_console (const char*, int, int, int);
void crivello_old (lint, const char*, bool);
void crivello_new (lint, const char*, bool, lint, bool);
void crivello_list (lint, const char*, bool, lint, bool);
int main (int argc, char **argv) {
	char bff[MAX_BUF], copt, filename[MAX_PATH];
	lint n = -1, p = 0;
	int arg_index = 0;
	bool toll = false, st = false, restore = false;
	lint ob = 0;
	algorithm tp = new_a;
	type ty = crivello_t;
	memset (bff, 0, sizeof (bff));
	memset (filename, 0, sizeof (filename));
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"os-type", no_argument, 0, 'z'},
		{"license", no_argument, 0, 'l'},
		{"show-state", no_argument, 0, 's'},
		{"type", required_argument, 0, 't'},
		{"algorithm", required_argument, 0, 'x'},
		{"toll", no_argument, 0, 'T'},
		{"out-file", required_argument, 0, 'o'},
		{"last-number", required_argument, 0, 'n'},
		{"prog-hop", required_argument, 0, 'p'},
		{"reload-last", no_argument, 0, 'r'},
		{0, 0, 0, 0}
	};
	while ((copt = getopt_long (argc, argv, "hvzlst:x:To:n:p:r", long_options, &arg_index)) != -1) {
		switch (copt) {
			case 'l':
				print_license ();
				exit (0);
			case 'v':
				print_version ();
				exit (0);
			case 'h':
				print_infos ();
				exit (0);
		 	case 'z':
                		welcome ();
                		exit (0);
                	case 'n':
				ob++;
				n = (lint)atol (optarg);
				break;
                	case 'o':
                		ob++;
                		strncpy (filename, optarg, sizeof (filename) - 1);
                		break;
                	case 'T':
                		toll = true;
                		break;
                	case 'x':
                		if (strcmp (optarg, "list") == 0)
                			tp = list_a;
                		else if (strcmp (optarg, "old") == 0)
                			tp = old_a;
                		else if (strcmp (optarg, "new") == 0)
                			tp = new_a;
                		else {
                			print_param_err ();
                			exit (5);
                		}
                		break;
                	case 't':
                		if (strcmp (optarg, "prime") == 0 || strcmp (optarg, "p") == 0)
                			ty = prime_t;
                		else if (strcmp (optarg, "crivello") == 0 || strcmp (optarg, "c") == 0)
                			ty = crivello_t;
                		else {
                			print_param_err ();
                			exit (4);
                		}
                		break;
                	case 'p':
                		p = (lint)atoi (optarg);
                		break;
                	case 's':
                		st = true;
                		break;
                	case 'r':
                		restore = true;
                		break;
			default:
				print_param_err ();
				exit (1);
		}
	}
	if (n < 2 || ob != 2 || p > 100 || (p < 1 && tp != new_a) || (p < -1 && tp == new_a) || (restore && ty != prime_t) || (toll && ty != crivello_t) || (st && (tp != new_a || ty != crivello_t))) {
		print_param_err ();
		exit (2);
	}
	if (ty == crivello_t)
		switch (tp) {
			case old_a:
				crivello_old (n, filename, toll);
				break;
			case new_a:
				crivello_new (n, filename, toll, p, st);
				break;
			case list_a	:
				crivello_list (n, filename, toll, p, st);
				break;
			default:
				fprintf (stderr, "Error.\n");
				exit (4);
		}
	else {
		start_nprimi_al (filename, restore, n);
	}
	return 0;
}
void print_list (primolist list, FILE *f, bool toll) {
	while (list != NULL) {
		fprintf (f, "%ld\n", list->n);
		if (toll) {
			char bff[MAX_BUF];
			memset (bff, 0, sizeof (bff));
			snprintf (bff, sizeof (bff) - 1, "%ld", list->n);
			write_to_console (bff, RESET, 0, 0);
			write_to_console ("\n", RESET, 0, 1);
		}
		list = list->next;
	}
}
primolist create (lint n) {
	primolist t = (primolist)malloc (sizeof (primo));
	t->n = n;
	t->next = NULL;
	return t;
}
primolist init_list (primolist list, lint n, primolist *start) {
	primolist tmp = NULL;
	lint i = 11;
	list = create (2);
	tmp = list;
	tmp->next = create (3);
	tmp = tmp->next;
	tmp->next = create (5);
	tmp = tmp->next;
	tmp->next = create (7);
	tmp = tmp->next;
	(*start) = tmp;
	while (i <= n) {
		while ((i % 2) == 0 || (i % 3) == 0 || (i % 5) == 0 || (i % 7) == 0)
			i++;
		if (i > n)
			break;
		tmp->next = create (i);
		tmp = tmp->next;
		i++;
	}
	return list;
}

void checklist (primolist list, lint num) {
	primolist idx = NULL, tmp = NULL;
	idx = list->next;
	while (idx != NULL) {
		if (idx->n % num == 0) {
			if (idx->next != NULL) {
				tmp = idx->next;
				idx->n = tmp->n;
				idx->next = tmp->next;
				free (tmp);
			} else
				idx = NULL;
		}
		if (idx == NULL)
			break;
		idx = idx->next;
	}
}

void free_list (primolist list) {
	while (list != NULL) {
		primolist tmp = list->next;
		free (list);
		list = tmp;
	}
}

void crivello_list (lint n, const char *filename, bool toll, lint p, bool st) {
	char bff[MAX_BUF];
	time_t stime, ftime;
	double dtime;
	FILE *o = NULL;
	primolist list = NULL, idx = NULL;
	lint prog = 0, lprog = -1;
	stime = time (NULL);
	write_to_console ("Inizializing number table...\n", GREEN, 0, 1);
	list = init_list (list, n, &idx);
	write_to_console ("Inizialized.\n", BLUE, 0, 1);
	write_to_console ("Starting calculating...\n", GREEN, 0, 1);
	while (idx != NULL && (idx->n * idx->n) <= n) {
		prog = idx->n * idx->n * 100 / n;
		print_percentage (&prog, &lprog, 1, "Calculating...");
		checklist (idx, idx->n);
		idx = idx->next;
	}
	prog = 100;
	print_percentage (&prog, &lprog, 1, "Calculating...");
	write_to_console ("\n", RESET, 0, 1);
	ftime = time (NULL);
	dtime = difftime (ftime, stime);
	memset (bff, 0, sizeof (bff));
	snprintf (bff, sizeof (bff) - 1, "%.0lf", dtime);
	write_to_console ("Calculation last: ", BLUE | RED, 1, 0);
	write_to_console (bff, BLUE | RED, 1, 0);
	write_to_console ("s\n", BLUE | RED, 1, 1);
	write_to_console ("Calculation finished.\n", BLUE, 0, 1);
	write_to_console ("Starting writing on file...\n", GREEN, 0, 1);
	if ((o = fopen (filename, "w+t")) == NULL) {
		fprintf (stderr, "Error.");
		exit (3);
	}
	print_list (list, o, toll);
	write_to_console ("Finished.\n", BLUE, 0, 1);
	free_list (list);
	fflush (o);
	fclose (o);
}

void crivello_new (lint n, const char *filename, bool toll, lint p, bool st) {
	time_t stime, ftime;
	double dtime;
	char bff[MAX_BUF], glob[MAX_BUF];
	char *list;
	lint i = 0, j = 0, x = 0, y = 0;
	lint prog = 0, lprog = -1, nsize = 0, pprog = 0, lpprog = -1;
	FILE *o = NULL;
	stime = time (NULL);
	write_to_console ("Inizializing number table...\n", GREEN, 0, 1);
	nsize = (n - 1) * sizeof (char);
	list = (char*)malloc (nsize);
	memset (list, 0, nsize);
	write_to_console ("Inizialized.\n", BLUE, 0, 1);
	write_to_console ("Starting calculating...\n", GREEN, 0, 1);
	x = 2;
	for (i = 0; x * x < n; i++) {
		x = i + 2;
		if (p > 0) {
			prog = x * x * 100 / (n - 2);
			if (st) {
				memset (glob, 0, sizeof (glob));
				snprintf (glob, sizeof (glob) - 1, "Global %ld/%ld", x, (lint)sqrt (n - 2) + 1);
				print_double_percentage (&prog, &lprog, glob, &pprog, &lpprog, "Partial", 1, p, 0);
			} else
				print_double_percentage (&prog, &lprog, "Global", &pprog, &lpprog, "Partial", 1, p, 0);
		} else if (p == 0) {
			prog = x * x * 100 / (n - 2);
			print_percentage (&prog, &lprog, 1, "Global");
		}
		if (list[i] == 0) {
			for (j = (i + 1); j < (n - 1); j++) {
				y = j + 2;
				if (list[j] == 0) {
					if (p > 0) {
						pprog = j * 100 / (n - 2);
						if (st)
							print_double_percentage (&prog, &lprog, glob, &pprog, &lpprog, "Local", 1, p, 0);
						else
							print_double_percentage (&prog, &lprog, "Global", &pprog, &lpprog, "Local", 1, p, 0);
					}
					if (y % x == 0)
						list[j] = 1;
				}
			}
		}
	}
	prog = 100;
	pprog = 100;
	if (p > 0)
		print_double_percentage (&prog, &lprog, glob, &pprog, &lpprog, "Partial", 1, 1, 0);
	else
		print_percentage (&prog, &lprog, 1, "Global");
	write_to_console ("\n", RESET, 0, 1);
	ftime = time (NULL);
	dtime = difftime (ftime, stime);
	memset (bff, 0, sizeof (bff));
	snprintf (bff, sizeof (bff) - 1, "%.0lf", dtime);
	write_to_console ("Calculation last: ", BLUE | RED, 1, 0);
	write_to_console (bff, BLUE | RED, 1, 0);
	write_to_console ("s\n", BLUE | RED, 1, 1);
	write_to_console ("Calculation finished.\n", BLUE, 0, 1);
	write_to_console ("Starting writing on file...\n", GREEN, 0, 1);
	if ((o = fopen (filename, "w+t")) == NULL) {
		fprintf (stderr, "Error.\n");
		exit (3);
	}
	for (i = 0; i < (n - 1); i++)
		if (list [i] == 0) {
			snprintf (bff, sizeof (bff) - 1, "%ld\n", (i + 2));
			if (toll)
				write_to_console (bff, RED | BLUE, 1, 1);
			fprintf (o, "%s", bff);
		}
	write_to_console ("Finished.\n", BLUE, 0, 1);
	fflush (o);
	fclose (o);
}
void crivello_old (lint n, const char *filename, bool toll) {
	char bff[MAX_BUF];
	number *lnum;
	lint i = 0, j = 0;
	lint prog = 0, lprog = -1, nsize = 0;
	FILE *o = NULL;
	write_to_console ("Inizializing number table...\n", GREEN, 0, 1);
	nsize = (n - 1) * sizeof (number);
	lnum = (number*)malloc (nsize);
	memset (lnum, 0, nsize);
	for (i = 0; i < (n - 1); i++) {
		lnum[i].number = i + 2;
		lnum[i].flag = 0;
	}
	write_to_console ("Inizialized.\n", BLUE, 0, 1);
	write_to_console ("Starting calculating...\n", GREEN, 0, 1);
	for (i = 0; lnum[i].number * lnum[i].number < n; i++) {
		prog = lnum[i].number * lnum[i].number * 100 / (n - 2);
		print_percentage (&prog, &lprog, 1, "Global");
		if (lnum[i].flag == 0)
			for (j = (i + 1); j < (n - 1); j++)
				if (lnum[j].number % lnum[i].number == 0)
					lnum[j].flag = 1;
	}
	prog = 100;
	print_percentage (&prog, &lprog, 1, "Global");
	write_to_console ("\n", RESET, 0, 1);
	if ((o = fopen (filename, "w+t")) == NULL) {
		fprintf (stderr, "Error.\n");
		exit (3);
	}
	for (i = 0; i < (n - 1); i++)
		if (lnum[i].flag == 0) {
			snprintf (bff, sizeof (bff) - 1, "%ld\n", lnum[i].number);
			if (toll)
				write_to_console (bff, RED | BLUE, 1, 1);
			fprintf (o, "%s", bff);
		}
	write_to_console ("Finished.\n", BLUE, 0, 1);
	fflush (o);
	fclose (o);
}
void print_double_percentage (lint *global, lint *lastglobal, const char *globdesc, lint *part, lint *lastpart, const char *partdesc, lint globalhop, lint parthop, char force) {
	char w_prog[20];
	lint barw, i = 0;
	if (((*global % globalhop == 0) && (*global != *lastglobal)) || ((*part % parthop == 0) && (*part != *lastpart)) || force) {
		barw = BAR_WIDTH * (*part) / 100;
		write_to_console ("\r", RESET, 0, 0);
		write_to_console (" ", RESET, 0, 1);
		write_to_console ("\r", RESET, 0, 0);
		write_to_console (globdesc, BLUE | RED, 1, 0);
		write_to_console (": ", RESET, 1, 0);
		memset (w_prog, 0, sizeof (w_prog));
		snprintf (w_prog, sizeof (w_prog) - 1, "%ld%%", *global);
		write_to_console (w_prog, BLUE | RED, 1, 0); 
		write_to_console ("   |   ", RESET, 0, 0);
		write_to_console (partdesc, RESET, 1, 0);
            write_to_console (": [", RESET, 1, 0);
		for (i = 0; i < barw; i++)
			write_to_console ("=", RED, 1, 0);
			write_to_console (">", RED, 1, 0);
		for (i = 0; i < BAR_WIDTH - barw; i++)
			write_to_console ("-", BLUE, 0, 0);
            write_to_console ("] ", RESET, 1, 0);
            memset (w_prog, 0, sizeof (w_prog));
		snprintf (w_prog, sizeof (w_prog) - 1, "%ld%%   ", (*part));
            write_to_console (w_prog, (*part) < (lint)75 ? GREEN | BLUE : RED, 1, 1);
            (*lastglobal) = (*global);
            (*lastpart) = (*part);
            fflush (stdout);
	}
}
void print_percentage (lint *prog, lint *lastprog, lint proghop, const char *description) {
	lint i = 0;
	lint barw = 0;
	char w_prog[20];
	memset (w_prog, 0, sizeof (w_prog));
	barw = BAR_WIDTH * (*prog) / 100;
	if (((*prog) % proghop) == 0 && (*prog) != (*lastprog)) {
            write_to_console ("\r", RESET, 0, 0);
            write_to_console (description, RESET, 1, 0);
            write_to_console (": [", RESET, 1, 0);
			for (i = 0; i < barw; i++)
				write_to_console ("=", RED, 1, 0);
			write_to_console (">", RED, 1, 0);
			for (i = 0; i < BAR_WIDTH - barw; i++)
				write_to_console ("-", BLUE, 0, 0);
            write_to_console ("] ", RESET, 1, 0);
            snprintf (w_prog, sizeof (w_prog) - 1, "%ld%%", (*prog));
            write_to_console (w_prog, (*prog) < (lint)75 ? GREEN | BLUE : RED, 1, 1);
		fflush (stdout);
		(*lastprog) = (*prog);
	}
}
void print_infos () {
	write_to_console ("\ncrivello          -          a pi calculator\n", RED, 1, 0);
	
    	write_to_console ("\nDeveloped by Niccolo' Ferrari\n", RED | BLUE, 1, 0);
    	
    	write_to_console ("\n\nOPTIONS:\n", RESET, 1, 0);
    	
    	write_to_console ("\t--help, -h:\n\t\tShow the help.\n", RESET, 1, 1);
    	
    	write_to_console ("\t--version, -v:\n\t\tShow the version and the license.\n", RESET, 1, 1);
    	
    	write_to_console ("\t--license, -l:\n\t\tShow the license. (GNU GPL).\n", RESET, 1, 1);
    	
    	write_to_console ("\t--os-type, -z:\n\t\tShow the machine infos.\n", RESET, 1, 1);
    	
    	write_to_console ("\n", RESET, 0, 1);
    	
    	write_to_console ("\t--type=<option>, -t <option>:\n\t\tSelect the calculation mode:\n", RESET, 1, 1);
    	write_to_console ("\t\t<option> can be:\n", RESET, 1, 1);
    	write_to_console ("\t\t\t- \"crivello\", to calculate primes with crivello.\n", RESET, 1, 1);
    	write_to_console ("\t\t\t  [Or you can skip this option crivello is selected by default].\n", RESET, 1, 1);
    	write_to_console ("\t\t\t- \"prime\" or \"p\" to calculate primes with this alternative mode.\n\n", RESET, 1, 1);
    	
    	write_to_console ("\t--last-number=<number>, -n <number>:\n\t\tSpecify: if you chose \"crivello\" the last number to check, if you chose \"prime\" specify how much prime numbers you want.\n\n", RESET, 1, 1);
    	
    	write_to_console ("\t--out-file=<filename>, -o <filename>:\n\t\tName of the output file.\n\n", RESET, 1, 1);
    	
    	write_to_console ("\t--algorithm=<algorithm>, -x <algorithm>:\n\t\tIf you chose \"crivello\" specify the algorithm:\n", RESET, 1, 1);
    	write_to_console ("\t\t\t- \"new\": if not specified setted by default.\n", RESET, 1, 1);
    	write_to_console ("\t\t\t- \"list\": fastest but require lot of memory.\n", RESET, 1, 1);
    	write_to_console ("\t\t\t- \"old\": deprecated.\n\n", RESET, 1, 1);
    	
    	write_to_console ("\t--show-state, -s:\n\t\tShow details of the state. Works only in \"crivello\" with \"new\" algorithm\n", RESET, 1, 1);
    	
    	write_to_console ("\t--toll, -T:\n\t\tPrint result on screen, works only on \"crivello\".\n\n", RESET, 1, 1);
    	
    	write_to_console ("\t--prog-hop=<number>, -p <number>:\n\t\tSpecify when refresh the local progress bar and percentage.\n", RESET, 1, 1);
    	write_to_console ("\t\tThis work only on \"crivello\" with \"new\" algorithm:\n\t\tUse 0 value to not display local progress (FASTER),\n\t\tor -1 to NOT display progress (FASTEST).\n\t\tThe positive number have to be between 1 and 100.\n\n", RESET, 1, 1);
    	
    	write_to_console ("\t--reload-last, -r:\n\t\tRestart calculation from last session\n", RESET, 1, 1);
    	
    	write_to_console ("\n\nEXAMPLE:", RESET, 0, 0);
    	write_to_console ("\n\tprime_numbers -n 1000000 -t crivello -x new -o outputfile.txt -p 25\n", RESET, 0, 0);
    	
    	write_to_console ("\n\nCONTACT:", RESET, 1, 0);
    	
    	write_to_console ("\n\tniccolo.ferrari@hotmail.it\n", RED | GREEN, 1, 1);
}
void print_license () {
	write_to_console ("This software is under GNU GPL license. See http://minegrado.info/DWN/LICENSE.txt for details.\n", RED | BLUE, 1, 1);
}
void welcome () {
	#if defined (__linux__)
    	write_to_console ("Hi Linux user!\n", RED, 0, 1);
	#elif defined (__FreeBSD__)
    	write_to_console ("Hi FreeBSD user!\n", RED, 0, 1);
	#elif defined (__OpenBSD__)
    	write_to_console ("Hi OpenBSD user!\n", RED, 0, 1);
	#elif defined (__NetBSD__)
    	write_to_console ("Hi NetBSD user!\n", RED, 0, 1);
	#elif defined (__DragonFly__)
    	write_to_console ("Hi DragonFlyBSD user!\n", RED, 0, 1);
	#elif defined (__sun)
    	write_to_console ("Hi Solaris user!\n", RED, 0, 1);
	#elif (defined (__APPLE__) && defined (__MACH__))
    	write_to_console ("Hi Mac user!\n", RED, 0, 1);
	#elif defined (__unix__)
    	write_to_console ("Hi Unix user!\n", RED, 0, 1);
	#elif defined (_WIN64)
    	write_to_console ("Hi Windows 64bit user!\n", RED, 0, 1);
    	#elif defined (_WIN32)
    	write_to_console ("Hi Windows 32bit user!\n", RED, 0, 1);
	#else
    	write_to_console ("S/O not reconized\n", RED, 0, 1);
	#endif
}
void print_version () {
	print_license ();
    	write_to_console ("crivello version " VERSION " developed by OmegaSoftware (C) a Niccolo' Ferrari company, on 06 feb 2015, with GNU GPL license.\nFor info:\nniccolo.ferrari@hotmail.it\nomegasoftware.altervista.org\nminegrado.info\nminegrado.info/DWN\n", RESET, 1, 1);
}
void print_param_err () {
    	write_to_console ("Param error: try \"prime_numbers --help\"\n", RED, 1, 1);
}
void write_to_console (const char* _string, int color, int bold, int flush) {
	#if defined (UNIX_LIKE)
	if (bold)
		printf (F_BOLD);
    	switch (color) {
        	case RED:
		  	printf (C_RED "%s", _string);
            	break;
        	case BLUE:
            	printf (C_BLUE "%s", _string);
            	break;
        	case GREEN:
            	printf (C_GREEN "%s", _string);
            	break;
        	case RED | BLUE:
            	printf (C_MAGENTA "%s", _string);
            	break;
        	case RED | GREEN:
            	printf (C_YELLOW "%s", _string);
            	break;
        	case GREEN | BLUE:
            	printf (C_CYAN "%s", _string);
            	break;
        	default:
            	printf ("%s", _string);
            	break;
    }
    printf (C_RESET);
    #elif defined (NT_LIKE)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;

    WORD w_color = saved_attributes;
    switch (color) {
        case RED:
            w_color = FOREGROUND_RED;
            break;
        case BLUE:
            w_color = FOREGROUND_BLUE;
            break;
        case GREEN:
            w_color = FOREGROUND_GREEN;
            break;
        case RED | BLUE:
            w_color = FOREGROUND_RED | FOREGROUND_BLUE;
            break;
        case RED | GREEN:
            w_color = FOREGROUND_RED | FOREGROUND_GREEN;
            break;
        case GREEN | BLUE:
            w_color = FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;
        default:
            w_color = saved_attributes;
            break;
    }
    if (bold)
        w_color |= FOREGROUND_INTENSITY;
    SetConsoleTextAttribute(hConsole, w_color);

    printf("%s", _string);

    SetConsoleTextAttribute(hConsole, saved_attributes);
	#endif
	if (flush)
        fflush (stdout);
}
