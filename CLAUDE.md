## Build
- Командата е: .\build_clean_win64.bat NOPAUSE  (от корена на framework-а; без NOPAUSE увисва на pause)
- Билдът е дълъг: пускай го във фонов режим с пренасочване към build.log и следи лога.
- Скриптът винаги излиза с код 0. Успех = в лога има "Build done!" и няма "FATAL BUILD ERROR!".
- Резултатът е в build_win64\Output. Сървърът се стартира с amc_server.exe оттам.
- Ако скриптът каже "Please rebuild client!", спри и ми кажи, не се опитвай да го поправяш сам.

## Проект
- Моят проект е Examples\MySLM (копие на LPBFSystem); CMAKE_AMCF_PLUGINNAME в главния CMakeLists.txt сочи към него.
- Examples\MySLM е отделно git хранилище; промените се виждат с git diff, пуснат там.
- Скенер: RTC5 хардуер, но ползваме RTC6 драйвера на AMCF (scanlab).
- PLC: HNC HCG през Modbus TCP, чрез TCPIP драйвера (име "plc" в config.xml).
- mcplugin_plc.cpp е с Modbus TCP (MBAP), без BuR код.
- Плъгините ползват file(GLOB *.cpp): никога не оставяй копия като "file - Copy.cpp".
- Не пипай bur/raylase/scanlabsmc/camera драйверите засега.
- След всяка промяна компилирай и покажи оставащите грешки.
