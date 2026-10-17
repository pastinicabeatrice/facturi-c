#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned int nrFactura;
    char* dataEmitere;
    char* beneficiar;
    float sumaDePlata;
    unsigned int nrZileScadenta;
} factura;

typedef struct nod {
    factura* inf;
    struct nod* next;
    struct nod* prev;
} nod;

nod* inserare(nod* cap, nod** coada, factura* f) {
    nod* nou = (nod*)malloc(sizeof(nod));
    if (nou == NULL) {
        return NULL;
    }
    nou->inf = (factura*)malloc(sizeof(factura));
    if (nou->inf == NULL) {
        free(nou);
        return NULL;
    }

    nou->inf->nrFactura = f->nrFactura;
    nou->inf->dataEmitere = (char*)malloc((strlen(f->dataEmitere) + 1) * sizeof(char));
    if (nou->inf->dataEmitere == NULL) {
        free(nou->inf);
        free(nou);
        return NULL;
    }
    strcpy(nou->inf->dataEmitere, f->dataEmitere);
    nou->inf->beneficiar = (char*)malloc((strlen(f->beneficiar) + 1) * sizeof(char));
    if (nou->inf->beneficiar == NULL) {
        free(nou->inf->dataEmitere);
        free(nou->inf);
        free(nou);
        return NULL;
    }
    strcpy(nou->inf->beneficiar, f->beneficiar);
    nou->inf->sumaDePlata = f->sumaDePlata;
    nou->inf->nrZileScadenta = f->nrZileScadenta;

    nou->prev = NULL;
    nou->next = NULL;

    if (cap == NULL) {
        cap = nou;
        *coada = nou;
    }
    else {
        nod* temp = cap;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = nou;
        nou->prev = temp;
        *coada = nou;
    }

    return cap;
}

void citireDate(nod** cap, nod** coada, const char* numeFisier) {
    FILE* file = fopen(numeFisier, "r");
    if (file == NULL) {
        printf("Fisierul nu poate fi deschis!\n");
        return;
    }
    int count = 0;
    while (!feof(file) && count < 5) {
        factura f;
        char buffer1[200], buffer2[200];
        if (fscanf(file, "%u %s %s %f %u", &f.nrFactura, buffer1, buffer2, &f.sumaDePlata, &f.nrZileScadenta) == 5) {
            f.dataEmitere = (char*)malloc((strlen(buffer1) + 1) * sizeof(char));
            f.beneficiar = (char*)malloc((strlen(buffer2) + 1) * sizeof(char));
            strcpy(f.dataEmitere, buffer1);
            strcpy(f.beneficiar, buffer2);

            *cap = inserare(*cap, coada, &f);
            count++;
        }
    }

    fclose(file);
}

void traversare(nod* cap) {
    nod* temp = cap;
    while (temp != NULL) {
        printf("Numarul facturii: %u\n Data la care a fost emisa: %s\n Beneficiarul facturii: %s\n Suma de plata: %.2f\n Numarul de zile pana la scadenta: %u\n",
            temp->inf->nrFactura, temp->inf->dataEmitere, temp->inf->beneficiar, temp->inf->sumaDePlata, temp->inf->nrZileScadenta);
        temp = temp->next;
    }
}

void dezalocare(nod* cap) {
    nod* temp = cap;
    while (temp != NULL) {
        nod* temp2 = temp->next;
        free(temp->inf->beneficiar);
        free(temp->inf->dataEmitere);
        free(temp->inf);
        free(temp);
        temp = temp2;
    }
}

void traversareInversa(nod* coada) {
    nod* temp = coada;
    while (temp != NULL) {
        printf("Numarul de zile pana la scadenta: %u\n Beneficiarul facturii: %s\n Suma de plata: %.2f\n Data la care a fost emisa: %s\n Numarul facturii: %u\n",
            temp->inf->nrZileScadenta, temp->inf->beneficiar, temp->inf->sumaDePlata, temp->inf->dataEmitere, temp->inf->nrFactura);
        temp = temp->prev;
    }
}

float totalFacturi(nod* cap, const char* beneficiar) {
    float total = 0;
    nod* temp = cap;
    while (temp != NULL) {
        if (strcmp(temp->inf->beneficiar, beneficiar) == 0) {
            total += temp->inf->sumaDePlata;
        }
        temp = temp->next;
    }

    return total;
}

void modificareSumea(nod* cap, unsigned int nrFactura, float nouaSuma) {
    nod* temp = cap;
    while (temp != NULL) {
        if (temp->inf->nrFactura == nrFactura) {
            temp->inf->sumaDePlata = nouaSuma;
            printf("Suma de plata pentru factura cu numarul %u a fost modificata la %.2f! \n ", nrFactura, nouaSuma);
            return;
        }
        temp = temp->next;
    }
    printf("Factura cu numarul %u nu a fost gasita in sistem, va rugam introduceti o alta valoare. \n", nrFactura);
}

void stergeFcatura(nod** cap, nod** coada, unsigned int prag) {
    nod* temp = *cap;
    while (temp != NULL) {
        nod* urm = temp->next;
        if (temp->inf->nrZileScadenta < prag) {
            if (temp->prev != NULL) {
                temp -> prev->next = temp->next;
            }
            else {
                *cap = temp->next;
            }
            if (temp->next != NULL) {
                temp->next->prev = temp->prev;
            }
            else {
                *coada = temp->prev;
            }
            free(temp->inf->beneficiar);
            free(temp->inf->dataEmitere);
            free(temp->inf);
            free(temp);
        }
        temp = urm;
    }
    printf("Facturile cu numarul de zile pana la scadenta mai mic decat %u au fost sterse!\n", prag);
}

int main() {
    nod* cap = NULL;
    nod* coada = NULL;
    printf("*** Sistem de gestiune a facturilor ***\n");
    citireDate(&cap, &coada, "facturi.txt");
    printf("\n");

    char beneficiar[200];
    printf("Introduceti beneficiarul pentru care doriti intreaga suma de plata: \n");
    if (scanf("%199s", beneficiar) != 1) {
        strcpy(beneficiar, "Vodafone");
    }
    float totalDePlata = totalFacturi(cap, beneficiar);
    printf("Totalul de plata pentru %s este: %.2f\n", beneficiar, totalDePlata);
    printf("\n");

    unsigned int nrFactura;
    float nouaSuma;
    printf("Introduceti numarul facturii pentru care doriti actualizarea sumei de plata: ");
    if (scanf("%u", &nrFactura) != 1) {
        nrFactura = 2;
    }
    printf("Introduceti noua suma: ");
    if (scanf("%f", &nouaSuma) != 1) {
        nouaSuma = 1500;
    }
    modificareSumea(cap, nrFactura, nouaSuma);
    printf("\n");

    unsigned int prag;
    printf("Introduceti numarul minim de zile de scadenta acceptat pentru a mai mentine date in sistem: \n");
    if (scanf("%u", &prag) != 1) {
        prag = 5;
    }
    stergeFcatura(&cap, &coada, prag);

    traversare(cap);
    printf("\n");
    traversareInversa(coada);
    printf("\n");
    dezalocare(cap);

    return 0;
}
