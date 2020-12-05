## Projekt z Telemedycyny 

>Akwizycja, zapis i przesyłanie sygnału EKG do bazy danych Heroku.
  * Moduł AD8232 - rejestracja sygnału,
  * Moduł czytnika kart SD - zapis,
  * Moduł WiFi ESP8266 - przesyłanie - nie udało się więc powstała aplikacja okienkowa, zakłada się, że urządzenie przynajmniej raz dziennie jest w domu, gdzie możemy przesłać dane do bazy.

![Schemat połaczeń](/schemat.png)

### Program Arduino do zapisywania 5 minutowych sygnałów do plików

>Folder **Projekt Arduino/sd_1**.

### Próby przesłania danych za pomocą ESP8266 do bazy danych Heroku

>Folder **Testowanie modułu WiFi/test_ESP1**.

### Programy do sprawdzania, co jest na karcie SD i czy działa

>Folder **Testowanie modułu czytnika kart SD**

![Prototyp](/prototyp.jpg)

### Aplikacja okienkowa WPF C# do przesyłania danych

>https://github.com/KamilSuchanek95/ZapisEKG_WPF

### Filmiki, które musiałem oddać razem z sprawozdaniem


[![Złożenie urządzenia 1](https://img.youtube.com/vi/SpOnCm-p1CQ/0.jpg)](https://www.youtube.com/watch?v=SpOnCm-p1CQ)

[![Złożenie, późniejsze włożenie karty SD, reakcja urządzenia na monitorze](https://img.youtube.com/vi/GMcE6DthpAg/0.jpg)](https://www.youtube.com/watch?v=GMcE6DthpAg)

[![Widok Monitora](https://img.youtube.com/vi/ndEQ74grKH0/0.jpg)](https://www.youtube.com/watch?v=ndEQ74grKH0)

[![Wgrywanie przykładowych rekordów do bazy danych](https://img.youtube.com/vi/nCj1PFEzOXY/0.jpg)](https://www.youtube.com/watch?v=nCj1PFEzOXY)

