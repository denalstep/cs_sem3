# cs_sem3

**Task 1**

Написать программу, которая реализует корректную работу сервера с несколькими клиентами с помощью именованных каналов (FIFO).

Схема:
Сервер создает именованный канал с определенным именем, открывая его на чтение, а запускаемые после клиенты открывают его на запись и отправляют серверу команды и необходимые данные (имя файла/PID). Клиенты берут в качестве аргумента командной строки имя файла, передают его серверу, сервер считывает содержимое файла и передает его клиенту, который распечатывает переданное содержимое на экран. Сервер должен корректно завершаться по нажатию ctr+c.
Программы должны освобождать выделенные ресурсы при любом исходе. Работа сервера с клиентами может идти поочередно в одном процессе.
