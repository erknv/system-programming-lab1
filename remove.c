/*
 * Лабораторная работа №1 (средний уровень): удаление каталога.
 * Эркинов Чынсултан, ПИ(ДОТ)-1-23
 *
 * Рекурсивно удаляет указанный каталог (по умолчанию Lab1_Erkinov).
 * Используются только вызовы из раздела 1.2: scandir, readlink, unlink, rmdir.
 *
 * Сборка:  gcc -Wall -Wextra -o remove remove.c
 * Запуск:  ./remove [каталог]
 */
#define _DEFAULT_SOURCE
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int errors = 0;

/* Пропускаем "." и ".." */
static int not_dots(const struct dirent *e)
{
    return strcmp(e->d_name, ".") != 0 && strcmp(e->d_name, "..") != 0;
}

static void remove_tree(const char *path)
{
    struct dirent **list;
    int n = scandir(path, &list, not_dots, alphasort);
    if (n == -1) {
        fprintf(stderr, "Ошибка scandir(\"%s\"): ", path);
        perror(NULL);
        errors++;
        return;
    }

    for (int i = 0; i < n; i++) {
        char child[PATH_MAX];
        snprintf(child, sizeof child, "%s/%s", path, list[i]->d_name);

        if (list[i]->d_type == DT_DIR) {
            /* Настоящий каталог — сначала очищаем его содержимое */
            remove_tree(child);
        } else {
            if (list[i]->d_type == DT_LNK) {
                /* Символьную ссылку НЕ разыменовываем, удаляем саму ссылку */
                char target[PATH_MAX];
                ssize_t len = readlink(child, target, sizeof target - 1);
                if (len >= 0) {
                    target[len] = '\0';
                    printf("[sym ] %s -> %s\n", child, target);
                }
            } else {
                printf("[file] %s\n", child);
            }
            if (unlink(child) == -1) {
                fprintf(stderr, "Ошибка unlink(\"%s\"): ", child);
                perror(NULL);
                errors++;
            }
        }
        free(list[i]);
    }
    free(list);

    /* rmdir удаляет только пустой каталог */
    if (rmdir(path) == -1) {
        fprintf(stderr, "Ошибка rmdir(\"%s\"): ", path);
        perror(NULL);
        errors++;
    } else {
        printf("[dir ] %s\n", path);
    }
}

int main(int argc, char *argv[])
{
    const char *root = argc > 1 ? argv[1] : "Lab1_Erkinov";
    remove_tree(root);
    if (errors) {
        fprintf(stderr, "Завершено с ошибками: %d\n", errors);
        return EXIT_FAILURE;
    }
    printf("Готово: каталог %s удалён.\n", root);
    return EXIT_SUCCESS;
}
