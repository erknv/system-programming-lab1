/*
 * Лабораторная работа №1. Файлы и каталоги (средний уровень).
 * Эркинов Чынсултан, ПИ(ДОТ)-1-23
 *
 * Программа создаёт каталог Lab1_Erkinov со всем содержимым.
 * Для работы с файловой системой используются только вызовы
 * из раздела 1.2: mkdir, open, close, read, write, lseek, link, symlink.
 *
 * Сборка:  gcc -Wall -Wextra -o create create.c
 * Запуск:  ./create
 */
#define _DEFAULT_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ROOT "Lab1_Erkinov"

/* Вывести ошибку системного вызова и завершить программу. */
static void die(const char *call, const char *path)
{
    fprintf(stderr, "Ошибка %s(\"%s\"): ", call, path);
    perror(NULL);
    exit(EXIT_FAILURE);
}

/* write может записать меньше, чем просили, поэтому пишем в цикле. */
static void write_all(int fd, const void *buf, size_t count, const char *path)
{
    const char *p = buf;
    while (count > 0) {
        ssize_t n = write(fd, p, count);
        if (n == -1)
            die("write", path);
        p += n;
        count -= (size_t)n;
    }
}

/* Создать новый файл. O_EXCL — ошибка, если файл уже существует. */
static int new_file(const char *path)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd == -1)
        die("open", path);
    return fd;
}

static void make_dir(const char *path)
{
    if (mkdir(path, 0755) == -1)
        die("mkdir", path);
    printf("[dir ] %s\n", path);
}

static void make_empty(const char *path)
{
    close(new_file(path));
    printf("[file] %s (пустой)\n", path);
}

static void make_text(const char *path, const char *text)
{
    int fd = new_file(path);
    write_all(fd, text, strlen(text), path);
    close(fd);
    printf("[file] %s (текст, %zu байт)\n", path, strlen(text));
}

/* N случайных байт: читаем из /dev/urandom и пишем в файл. */
static void make_random(const char *path, size_t size)
{
    char buf[512];
    int src = open("/dev/urandom", O_RDONLY);
    if (src == -1)
        die("open", "/dev/urandom");
    int fd = new_file(path);
    size_t left = size;
    while (left > 0) {
        size_t chunk = left < sizeof buf ? left : sizeof buf;
        ssize_t n = read(src, buf, chunk);
        if (n <= 0)
            die("read", "/dev/urandom");
        write_all(fd, buf, (size_t)n, path);
        left -= (size_t)n;
    }
    close(src);
    close(fd);
    printf("[file] %s (%zu случайных байт)\n", path, size);
}

/*
 * N нулевых байт через lseek: переводим указатель на позицию N-1
 * и записываем один нулевой байт. Пропущенная область («дыра»)
 * по стандарту читается как нули.
 */
static void make_zero(const char *path, size_t size)
{
    int fd = new_file(path);
    if (size > 0) {
        if (lseek(fd, (off_t)size - 1, SEEK_SET) == -1)
            die("lseek", path);
        write_all(fd, "", 1, path);
    }
    close(fd);
    printf("[file] %s (%zu нулевых байт)\n", path, size);
}

/* Жёсткая ссылка: оба пути — относительно текущего каталога. */
static void make_hardlink(const char *target, const char *linkpath)
{
    if (link(target, linkpath) == -1)
        die("link", linkpath);
    printf("[hard] %s => %s\n", linkpath, target);
}

/* Символьная ссылка: цель — относительно каталога самой ссылки. */
static void make_symlink(const char *target, const char *linkpath)
{
    if (symlink(target, linkpath) == -1)
        die("symlink", linkpath);
    printf("[sym ] %s -> %s\n", linkpath, target);
}

int main(void)
{
    /* 1. Каталоги (родитель — раньше вложенных) */
    make_dir(ROOT);
    make_dir(ROOT "/docs");
    make_dir(ROOT "/docs/notes");
    make_dir(ROOT "/data");
    make_dir(ROOT "/backup");

    /* 2. Файлы */
    make_text(ROOT "/docs/info.txt", "Erkinov Chynsultan, PI(DOT)-1-23\n");
    make_text(ROOT "/docs/notes/todo.txt", "System programming lab 1\n");
    make_empty(ROOT "/docs/notes/empty.txt");
    make_random(ROOT "/data/random.bin", 1024);
    make_zero(ROOT "/data/zeros.bin", 256);

    /* 3. Ссылки (исходные файлы уже существуют) */
    make_hardlink(ROOT "/docs/info.txt", ROOT "/backup/info_copy.txt");
    make_hardlink(ROOT "/data/random.bin", ROOT "/backup/random_copy.bin");
    make_symlink("docs", ROOT "/docs_link");             /* на каталог */
    make_symlink("../docs/notes/todo.txt", ROOT "/backup/todo_link"); /* на файл */

    printf("Готово: каталог %s создан.\n", ROOT);
    return 0;
}
