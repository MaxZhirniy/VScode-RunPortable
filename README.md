# English
## Use the portable assembly comfortably!
If your VSCODE is downloaded on a removable disk, and you use your removable disk in development (for example, you also have python downloaded on the removable disk), then your config will become inoperative after the drive letter changes:
```json
"python.defaultInterpreterPath" : "E:/programs/python"
```
Now that the drive letter is D, all paths inside settings of VSCODE will stop working.

## Solution
* Place RunPortable.exe in folder with vscode installation
* Run RunPortable
* The program will go through the files specified in RunPortable.ini
* On the first launch, the program will create RunPortable.ini with standard paths to configs (GlobalStorage.json, settings.json and other)
* After correcting each file from RunPortable.ini, the program will launch Code.exe
* Great! VSCode is open and now we can continue to write code

# Russian
## Пользуйтесь портативной сборкой с комфортом!
Если ваш VSCODE скачан на съемный диск, и вы используете свой съемный диск в разработке (например, у вас на съемном диске также загружен Python), то ваш конфиг станет нерабочим после смены буквы диска:
```json
"python.defaultInterpreterPath": "E:/programs/python"
```
Теперь, когда буква диска — D, все пути внутри настроек VSCODE перестанут работать.

## Решение
* Поместите RunPortable.exe в папку с установкой vscode.
* Запустите RunPortable
* Программа будет просматривать файлы, указанные в RunPortable.ini.
* При первом запуске программа создаст RunPortable.ini со стандартными путями к конфигам (GlobalStorage.json, settings.json и другие)
* После исправления каждого файла из RunPortable.ini программа запустит Code.exe
* Готово! VSCode открыт и теперь мы можем продолжить писать код
