#!/bin/sh
# Лабораторная работа №1. Создание той же структуры командами оболочки.
# Эркинов Чынсултан, ПИ(ДОТ)-1-23
# Запуск: sh create.sh
set -e
R=Lab1_Erkinov

if [ -e "$R" ]; then
    echo "Каталог $R уже существует — сначала удалите его (./remove)" >&2
    exit 1
fi

# 1. Каталоги
mkdir "$R"
mkdir "$R/docs" "$R/docs/notes" "$R/data" "$R/backup"

# 2. Файлы
printf 'Erkinov Chynsultan, PI(DOT)-1-23\n' > "$R/docs/info.txt"
printf 'System programming lab 1\n'        > "$R/docs/notes/todo.txt"
touch "$R/docs/notes/empty.txt"
head -c 1024 /dev/urandom > "$R/data/random.bin"
head -c 256  /dev/zero    > "$R/data/zeros.bin"

# 3. Ссылки
ln "$R/docs/info.txt"   "$R/backup/info_copy.txt"     # жёсткая
ln "$R/data/random.bin" "$R/backup/random_copy.bin"   # жёсткая
ln -s docs                     "$R/docs_link"          # символьная на каталог
ln -s ../docs/notes/todo.txt   "$R/backup/todo_link"   # символьная на файл

echo "Готово: каталог $R создан."
