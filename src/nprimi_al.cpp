#include "crivello_primi.hpp"

int P = 0;
int P_p = 0;
int P_f = 0;
lint *n_primi;

void print(int p, int p_p, int p_f){                                            //Funzione per la stampa a video dei progressi
    #if defined (UNIX_LIKE)
    system("clear");                                                              //Pulisce lo schermo
    #elif defined (NT_LIKE)
    system("cls");
    #endif
    cout << "Calcolo in corso..." << endl;
    printf("%i%c\n%c%i%c\n%c%i%c\n", p, '%', '\t', p_p, '%', '\t', p_f, '%');   //Stampa la percentuale di avanzamento
}

lint * getPrime(lint n, lint from = 0, lint startAt = 0){           //Funzione per il calcolo dei numeri primi
    if(n > 0){
        n_primi = new lint [n];
        lint i = 0;
        int c;
        lint j;
        if(startAt > 5){
            if(((startAt - 1) % 6) == 0){
                j = (lint)((startAt - 1) / 6);
                c = 5;
            }else{
                j = (lint)((startAt - 5) / 6) + 1;
                c = 1;
            }
        }else{
            j = 0;
            c = 5;
        }
        while(i < (n - from)){
            bool state = true;
            lint n_primo = (6 * j + c);
            lint rq = sqrt(n_primo);
            lint k = 0, l = 5;
            if((from == 0) && (startAt == 0) && (i == 0)){
                n_primi[i++] = 2;
                if(n > 1) n_primi[i++] = 3;
            }
            while(((6 * k + l) <= rq) && (state == true)){
                if((n_primo % (6 * k + l)) == 0) state = false;
                if(l == 5){
                    l = 1;
                    k++;
                }else{
                    l = 5;
                }
            }
            if(state){
                n_primi[i++] = n_primo;
                int p_t = (int)((100 * i) / (n - from));
                if(P_p == (p_t - 10)){
                    P_p = p_t;
                print(P, P_p, P_f);
                }
            }
            if(c == 1){
                c = 5;
            }
            else{
                c = 1;
                j++;
            }
        }
        
    }else{
        *n_primi = -1;
    }
    return n_primi;
    
}

void pFile(lint * s, lint num, const char *filename){
    FILE * file;
    file = fopen(filename, "a+");
    if(*s != -1){
        lint len = num;
        for(lint i = 0; i < len; i++){
            fwprintf(file, L"%" PRIi64 "\n", s[i]);
            //fprintf(file, "%li\n", s[i]);
            int p_t = 100;
            if (len > 1)
            	p_t = (int)((100 * i) / (len - 1));
            if(P_f == (p_t - 10)){
                P_f = p_t;
            print(P, P_p, P_f);
            }
        }
    }
    P_f = 0;
    P_p = 0;
    fclose(file);
}

void GoCalc(lint n, const char *filename, lint startAt = 0){
    lint *prime = NULL;
    if(n <= MIL){
        pFile(prime = getPrime(n, 0, startAt), n, filename);
    }else{
        lint m = (lint)(n / MIL);
        if(m != 0){
            lint last = startAt;
            for(lint i = 0; i < m; i++){
                n_primi = getPrime((i + 1) * MIL, i * MIL, last);
                last = n_primi[MIL - 1];
                pFile(n_primi, MIL, filename);
                if(P != (int)((100 * (i + 1)) / m)){
                    P = (int)((100 * (i + 1)) / m);
                P_p = 0;
                print(P, P_p, P_f);
                }
                prime = n_primi;
            }
            if((n % MIL) != 0){
                pFile(getPrime(n , m * MIL, last), n - (m * MIL), filename);
            }
        }else{
            pFile(prime = getPrime(n, 0, startAt), n, filename);
        }
    }
    if (*prime != -1 && prime != NULL) {
    	delete [] prime;
    }
}

lint * CountLines(const char *filename){
    string line;
    ifstream myfile(filename);
    lint * lines = new lint[2];
    lint LastPrime = 0;
    if (myfile.is_open())
    {
        int i = 0;
        while (myfile.good())
        {

            getline(myfile, line);
            LastPrime = (lint)atoll(line.c_str());
            if (LastPrime != 0) lines[1] = LastPrime;
            i++;
        }
    lines[0] = i;
    myfile.close();
    }
    else lines[0] = -1;
    return lines;
}

void start_nprimi_al (const char *filename, bool restore, lint num) {
    time_t start, end;
    double dif = 0.;
    lint n = num;
    time (&start);
    if (!restore){                                               //INIZIO NUOVO CALCOLO
        remove(filename);                                      //Rimuove il file precedente
        print(0, 0, 0);                                         //Stampa la percentuale di avanzamento del calcolo
        GoCalc(n, filename);                                              //Inizia calcolo
    } else {                                         //RIPRENDE UN CALCOLO
        cout << "Attendere, ripresa in corso..." << endl;
        lint *lines = CountLines(filename);                         //Numero di linee e ultimo numero primo calcolato
        if (n < lines[0]) {
        	fprintf (stderr, "Errore: il nuovo numero di di primi da calcolare deve essere >= %ld invece e' stato inserito: %ld\n", lines[0], n);
        	if (lines != NULL)
        		delete [] lines;
        	exit (10);
        }
        cout << "Ok, calcolo ripreso!" << endl;
        print(0, 0, 0);                                         //Stampa la percentuale di avanzamento del calcolo
        GoCalc(n - lines[0] + 1, filename, lines[1]);                     //Inizia calcolo
        if (lines != NULL)
        	delete [] lines;
    }
    time (&end);
    dif = difftime (end, start);
    printf("Calcolo avvenuto in %.2lf secondi.\n", dif);
    cout << "Fine." << endl;
}
