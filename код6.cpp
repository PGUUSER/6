#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

bool CheckAdj(int** matrix, int del1, int del2, int size) {
    if (del1 < 0 || del2 < 0 || del1 >= size || del2 >= size) return false;
    return matrix[del1][del2] == 1;
}

void create(int*** matrix, int size) {
    *matrix = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        (*matrix)[i] = (int*)calloc(size, sizeof(int));
    }
}

void fill(int** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = i; j < size; j++) {
            if (i == j) {
                matrix[i][j] = 0;
            }
            else {
                matrix[i][j] = rand() % 2;
                matrix[j][i] = matrix[i][j];
            }
        }
    }
}

void print(int** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d  ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Стягивание/отождествление двух вершин del1 и del2.
// compres = true означает, что это стягивание ребра (если ребро между ними есть, то можно сделать петлю на новом).
int** identification(int** matrix, int size, int del1, int del2, bool compres) {
    if (del1 < 0 || del2 < 0 || del1 >= size || del2 >= size || del1 == del2) return NULL;

    int newsize = size - 1;
    int** newMatrix = NULL;
    create(&newMatrix, newsize);

    // mapping: старые индексы -> новые
    int* map = (int*)malloc(size * sizeof(int));
    int idx = 0;
    for (int i = 0; i < size; i++) {
        if (i == del1 || i == del2) continue;
        map[i] = idx++;
    }
    int mergedIdx = newsize - 1;
    map[del1] = mergedIdx;
    map[del2] = mergedIdx;

    // копируем все пары (a,b) в новую матрицу через map
    for (int a = 0; a < size; a++) {
        for (int b = 0; b < size; b++) {
            int na = map[a];
            int nb = map[b];
            if (na == nb && (a == b)) {
                // это диагональ оригинала -> оставляем 0 (без петли по умолчанию)
                continue;
            }
            // при сжатии вершины: если хотя бы одна пара a,b отображается в same new ячейку
            // но мы хотим OR для рёбер, чтобы не потерять связи
            if (matrix[a][b]) {
                newMatrix[na][nb] = 1;
            }
        }
    }

    // если это стягивание (compres == false в твоём оригинале использовалось наоборот),
    // но сохраним поведение: если есть ребро между del1 и del2 и compres==false (как у тебя),
    // ставим петлю на новой вершине.
    if (!compres && matrix[del1][del2]) {
        newMatrix[mergedIdx][mergedIdx] = 1;
    }
    else {
        // обычно петля не нужна:
        newMatrix[mergedIdx][mergedIdx] = newMatrix[mergedIdx][mergedIdx]; // ничего
    }

    free(map);
    return newMatrix;
}

// Расщепление вершины split: добавляем новую вершину (индекс size) и переносим ребра
int** split(int** matrix, int size, int splitIndex) {
    if (splitIndex < 0 || splitIndex >= size) return NULL;

    int newsize = size + 1;
    int** splitMatrix = NULL;
    create(&splitMatrix, newsize);

    // копируем старую матрицу
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            splitMatrix[i][j] = matrix[i][j];

    // новая вершина имеет индекс = size
    int s = splitIndex;
    int newv = size;

    // для каждой вершины i:
    // сделаем, что новая вершина наследует рёбра исходной split-вершины,
    // и оригинал сохраняет свои ребра (вариант расщепления с дублированием связей).
    for (int i = 0; i < size; i++) {
        splitMatrix[i][newv] = matrix[i][s];   // ребро i -> new
        splitMatrix[newv][i] = matrix[s][i];   // ребро new -> i
    }

    // решаем связи между split и new: можно сделать их смежными
    splitMatrix[s][newv] = 1;
    splitMatrix[newv][s] = 1;
    splitMatrix[newv][newv] = 0;

    return splitMatrix;
}

int** disconnect(int** matrix1, int** matrix2, int maxSize) {
    int** resultMatrix = NULL;
    create(&resultMatrix, maxSize);
    for (int i = 0; i < maxSize; i++)
        for (int j = 0; j < maxSize; j++)
            resultMatrix[i][j] = (matrix1[i][j] == 1 || matrix2[i][j] == 1) ? 1 : 0;
    return resultMatrix;
}

int** connect(int** matrix1, int** matrix2, int maxSize) {
    int** resultMatrix = NULL;
    create(&resultMatrix, maxSize);
    for (int i = 0; i < maxSize; i++)
        for (int j = 0; j < maxSize; j++)
            resultMatrix[i][j] = (matrix1[i][j] == 1 && matrix2[i][j] == 1) ? 1 : 0;
    return resultMatrix;
}

int** ringsum(int** firstMatrix, int** secondMatrix, int maxSize) {
    int** resultMatrix = NULL;
    create(&resultMatrix, maxSize);
    for (int i = 0; i < maxSize; i++)
        for (int j = 0; j < maxSize; j++)
            resultMatrix[i][j] = (firstMatrix[i][j] ^ secondMatrix[i][j]) ? 1 : 0;
    return resultMatrix;
}

int main() {
    setlocale(LC_ALL, "Rus");
    srand((unsigned)time(NULL));

    int** matrix1 = NULL;
    int** matrix2 = NULL;
    int size1 = 0;
    int size2 = 0;
    int del1 = 0, del2 = 0;

    printf("Введите размер для первой матрицы: ");
    scanf("%d", &size1);
    printf("Введите размер для второй матрицы: ");
    scanf("%d", &size2);

    create(&matrix1, size1);
    fill(matrix1, size1);
    printf("\nМатрица-1\n");
    print(matrix1, size1);

    create(&matrix2, size2);
    fill(matrix2, size2);
    printf("\nМатрица-2\n");
    print(matrix2, size2);

    printf("\nОтождествление вершин графа M1:\n");
    printf("Номер первой вершины для отождествления М1: ");
    scanf("%d", &del1);
    printf("Номер второй вершины для отождествления М1: ");
    scanf("%d", &del2);

    int** identificationFirstMatrix = identification(matrix1, size1, del1, del2, false);
    if (identificationFirstMatrix) {
        printf("\nМатрица-1 после отождествления вершин: \n");
        print(identificationFirstMatrix, size1 - 1);
    }
    else printf("Ошибка отождествления M1\n");

    printf("\nОтождествление вершин графа M2:\n");
    printf("Номер первой вершины для отождествления М2: ");
    scanf("%d", &del1);
    printf("Номер второй вершины для отождествления М2: ");
    scanf("%d", &del2);

    int** identificationSecondMatrix = identification(matrix2, size2, del1, del2, false);
    if (identificationSecondMatrix) {
        printf("\nМатрица-2 после отождествления вершин: \n");
        print(identificationSecondMatrix, size2 - 1);
    }
    else printf("Ошибка отождествления M2\n");

    // Пример стягивания (с проверкой смежности)
    bool ok = false;
    while (!ok) {
        printf("\nВведите два удаляемых ребра М1(через пробел): ");
        scanf("%d %d", &del1, &del2);
        if (CheckAdj(matrix1, del1, del2, size1)) {
            int** ec = identification(matrix1, size1, del1, del2, true);
            if (ec) {
                print(ec, size1 - 1);
                ok = true;
            }
        }
        else {
            printf("Вершины не смежны\n");
        }
    }

    // Расщепление
    int splitIdx = 0;
    printf("\nИндекс расщепляемой вершины М1: ");
    scanf("%d", &splitIdx);
    int** sp = split(matrix1, size1, splitIdx);
    if (sp) {
        print(sp, size1 + 1);
    }
    else printf("Ошибка расщепления\n");

    // Пример объединения/пересечения/кольцевой суммы
    int sizeA, sizeB;
    printf("Введите количество элементов G1: ");
    scanf("%d", &sizeA);
    printf("Введите количество элементов G2: ");
    scanf("%d", &sizeB);
    int maxMatrixSize = (sizeA > sizeB) ? sizeA : sizeB;

    int** firstMatrix = NULL;
    int** secondMatrix = NULL;
    create(&firstMatrix, maxMatrixSize);
    create(&secondMatrix, maxMatrixSize);
    fill(firstMatrix, sizeA);
    fill(secondMatrix, sizeB);

    printf("G1:\n"); print(firstMatrix, maxMatrixSize);
    printf("G2:\n"); print(secondMatrix, maxMatrixSize);

    int** ru = disconnect(firstMatrix, secondMatrix, maxMatrixSize);
    printf("Объединение:\n"); print(ru, maxMatrixSize);

    int** ri = connect(firstMatrix, secondMatrix, maxMatrixSize);
    printf("Пересечение:\n"); print(ri, maxMatrixSize);

    int** rx = ringsum(firstMatrix, secondMatrix, maxMatrixSize);
    printf("Кольцевая сумма:\n"); print(rx, maxMatrixSize);

    // Здесь можно добавить освобождение памяти (free) при желании.

    return 0;
}
