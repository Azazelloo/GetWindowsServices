# GetWindowsServices
Приложение, позволяющее управлять службами Windows.
В состав входят: 
  -библиотека (dll), содержащая функции GetServicesList (возвращает vector объектов ServicesObj, содержащих информацию о службах), StopSrv (останавливает указанную службу),
StartSrv (запускает указанную службу), RestartSrv (перезапускает указанную службу);
  -клиент, имитирующий интерфейс пользователя (реализована статическая линковка dll)
Приложение написано с использованием паттерна MVVM. 
Для корректного считывания служб необходимо запускать отладчик/exe файл с правами администратора.
