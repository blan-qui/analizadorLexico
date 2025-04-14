/*
 *  Analizador L�xico para JSON simplificado
 *  Materia: Compiladores
 *  Tarea 1 - An�lisis L�xico
 * Integrantes: Benjamin Unrau Dyck
 *               Blanca Franco
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAMLEX 256
#define MAX_INDENT 128

FILE *archivo_fuente;
FILE *archivo_salida;

int numLinea = 1;
char lexema[TAMLEX];
int nivel_indentacion = 0;

void error_lexico(const char *mensaje) {
    fprintf(archivo_salida, "[ERROR] L�nea %d: %s\n", numLinea, mensaje);
}

void imprimir_indentacion() {
    int i;
    for (i = 0; i < nivel_indentacion; i++) {
        fprintf(archivo_salida, "\t");
    }
}

void imprimir_token(const char *token) {
    imprimir_indentacion();
    fprintf(archivo_salida, "%s\n", token);
}

void procesar_literal_cadena(int c) {
    int i = 0;
    lexema[i++] = c;
    while ((c = fgetc(archivo_fuente)) != EOF && c != '"') {
        lexema[i++] = c;
        if (i >= TAMLEX - 1) break;
    }
    if (c == '"') {
        lexema[i++] = c;
        lexema[i] = '\0';
        imprimir_token("STRING");
    } else {
        error_lexico("Cadena no cerrada");
    }
}

void procesar_numero(int c) {
    int i = 0, tiene_punto = 0, tiene_exp = 0;
    lexema[i++] = c;
    while ((c = fgetc(archivo_fuente)) != EOF) {
        if (isdigit(c)) {
            lexema[i++] = c;
        } else if (c == '.' && !tiene_punto) {
            lexema[i++] = c;
            tiene_punto = 1;
        } else if ((c == 'e' || c == 'E') && !tiene_exp) {
            lexema[i++] = c;
            tiene_exp = 1;
            c = fgetc(archivo_fuente);
            if (c == '+' || c == '-') {
                lexema[i++] = c;
                continue;
            } else if (isdigit(c)) {
                lexema[i++] = c;
                continue;
            } else {
                error_lexico("Exponente mal formado");
                return;
            }
        } else {
            ungetc(c, archivo_fuente);
            break;
        }
    }
    lexema[i] = '\0';
    imprimir_token("NUMBER");
}

void procesar_palabra(const char *palabra) {
    if (strcmp(palabra, "true") == 0 || strcmp(palabra, "TRUE") == 0)
        imprimir_token("PR_TRUE");
    else if (strcmp(palabra, "false") == 0 || strcmp(palabra, "FALSE") == 0)
        imprimir_token("PR_FALSE");
    else if (strcmp(palabra, "null") == 0 || strcmp(palabra, "NULL") == 0)
        imprimir_token("PR_NULL");
    else
        error_lexico("Palabra no reconocida");
}

void analizar() {
    int c;
    while ((c = fgetc(archivo_fuente)) != EOF) {
        if (isspace(c)) {
            if (c == '\n') {
                numLinea++;
            }
            continue;
        }

        if (c == '{') {
            imprimir_token("L_LLAVE");
            nivel_indentacion++;
        } else if (c == '}') {
            nivel_indentacion--;
            imprimir_token("R_LLAVE");
        } else if (c == '[') {
            imprimir_token("L_CORCHETE");
            nivel_indentacion++;
        } else if (c == ']') {
            nivel_indentacion--;
            imprimir_token("R_CORCHETE");
        } else if (c == ',') {
            imprimir_token("COMA");
        } else if (c == ':') {
            imprimir_token("DOS_PUNTOS");
        } else if (c == '"') {
            procesar_literal_cadena(c);
        } else if (isdigit(c)) {
            procesar_numero(c);
        } else if (isalpha(c)) {
            int i = 0;
            lexema[i++] = c;
            while ((c = fgetc(archivo_fuente)) != EOF && isalpha(c)) {
                lexema[i++] = c;
            }
            lexema[i] = '\0';
            if (c != EOF) ungetc(c, archivo_fuente);
            procesar_palabra(lexema);
        } else {
            char msg[64];
            sprintf(msg, "S�mbolo no reconocido: '%c'", c);
            error_lexico(msg);
        }
    }
    imprimir_token("EOF");
}

int main(int argc, char *argv[]) {
    archivo_fuente = fopen("fuente.txt", "r");
    archivo_salida = fopen("output.txt", "w");

    if (!archivo_fuente || !archivo_salida) {
        printf("Error al abrir los archivos fuente o salida.\n");
        return 1;
    }

    analizar();

    fclose(archivo_fuente);
    fclose(archivo_salida);
    return 0;
}

