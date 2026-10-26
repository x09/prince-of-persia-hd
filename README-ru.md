# PoPHD — Prince of Persia HD для Linux

<img width="1280" height="800" alt="Prince of Persia HD" src="https://github.com/user-attachments/assets/df854895-e0b6-4ed2-a6aa-c49b163af539" />

Проект начинался как попытка проанализировать оригинальные
[исходные коды Prince of Persia для Apple II](https://github.com/jmechner/Prince-of-Persia-Apple-II),
опубликованные Джорданом Мехнером, и переписать игру под Linux с использованием
GCC, Make, SDL2 и SDL2_image.

После трансляции ассемблерного кода и запуска оригинальной игры на Linux
от дальнейшего развития реализации на основе Apple II решили отказаться.

| 5% of the workflow | 90% of the workflow |
| :---: | :---: |
| <img width="40%" height="40%" alt="Apple II PoP" src="https://github.com/user-attachments/assets/f0be88ce-9561-4746-804c-9e752126bcda" /> |<img width="40%" height="40%" alt="Apple II PoP" src="https://github.com/user-attachments/assets/36dfd912-3869-42a0-a091-cf8301119ae1" /> |

Работа продолжилась на базе [SDLPoP](https://github.com/NagyD/SDLPoP) —
нативного порта на C/SDL, созданного на основе дизассемблированной DOS-версии.
Он стал основой нынешней игры. Ранний перенос с Apple II остался частью истории
проекта; исходный архив выпуска содержит поддерживаемую реализацию на базе DOS.

Для новой версии созданы HD-графика персонажей и окружения, заменены заставка
и саундтрек, доработаны отображение анимаций, ловушек и декораций. Разработка
и создание графики велись с помощью ИИ. Сохранены знакомые прыжки, бои на мечах
и головоломки, а у главного героя появились некоторые скрытые возможности :)

| Old graphics | HD |
| :---: | :---: |
| <img width="1156" height="715" alt="изображение" src="https://github.com/user-attachments/assets/2444443b-3973-4f59-9191-67107fc50530" /> | <img width="1152" height="714" alt="изображение" src="https://github.com/user-attachments/assets/cbaaa06f-feab-47b7-9b5d-9fa9bfcd18b6" /> |

<img width="1440" height="780" alt="prince" src="https://github.com/user-attachments/assets/62f7c8ad-4cea-433b-9db7-6174fd475593" />

<img width="1000" height="320" alt="characters" src="https://github.com/user-attachments/assets/cbce5826-d8ca-4490-994e-a85eca37a0ff" />


Проект: [github.com/x09/prince-of-persia-hd](https://github.com/x09/prince-of-persia-hd).
[English version](README.md).

## Стек и зависимости

PoPHD — нативное Linux-приложение на C (C99), собираемое GCC и GNU Make.
SDL2 отвечает за окно, ввод и звук, SDL2_image — за загрузку PNG-графики.
Для сборки нужны GCC, Make, pkg-config и пакеты разработки SDL2/SDL2_image.
В ALT Linux они называются:

```text
gcc make pkg-config libSDL2-devel libSDL2_image-devel
```

Готовые игровые ресурсы включены в комплект: для обычной сборки не нужны
генераторы изображений или загрузка дополнительных файлов из сети.

## Сборка и запуск

Из каталога проекта или распакованного исходного архива:

```sh
make -j4
./dos/pophd
```

HD-графика включена по умолчанию. Режим можно выбрать явно:

```sh
./dos/pophd --hd
./dos/pophd --dos
```

F10 переключает HD и оригинальную графику во время HD-сеанса.
Стрелки — движение, Shift — действия и удары мечом, Esc — меню,
Alt+Enter — полноэкранный режим, Ctrl+Q — выход.
Полная справка управления исходного движка: `dos/README.SDLPoP.md`.

Базовые проверки ресурсов и загрузки уровней, а также полное автоматическое
прохождение запускаются командами:

```sh
make check
make campaign-full-check
```

Проверка кампании охватывает концовку и сравнивает DOS/HD-трассы симуляции,
но не заменяет визуальную проверку всех возможных игровых ситуаций.

## Установка и RPM

Для установки во временный каталог без изменения системных файлов:

```sh
make install DESTDIR=/tmp/pophd-stage prefix=/usr
```

Для системной установки выполните `make install prefix=/usr` с правами
администратора. В меню приложений игра называется PoPHD, установленная команда
запуска — `pophd`. Пользовательские настройки и сохранения находятся в
`~/.prince-of-persia-hd`.

Архив `prince-of-persia-hd-1.0.tar.gz` и расположенный рядом
`prince-of-persia-hd.spec` предназначены для сборки RPM в ALT Linux.
Оба README включены в исходный архив и устанавливаются вместе
с документацией пакета.

## Авторы и лицензии

Оригинальная игра: Джордан Мехнер и команда оригинального Prince of Persia.
Основа нынешнего движка: Давид Надь и участники SDLPoP.
Лицензия SDLPoP GPL-3.0-or-later и исходные авторские уведомления сохранены.
Права на оригинальные игровые материалы остаются у их правообладателей;
GPL движка не меняет лицензию этих материалов. Подробности:
`dos/COPYING`, `dos/README.SDLPoP.md` и `packaging/ASSET_NOTICES.md`.
