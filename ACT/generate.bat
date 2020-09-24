
act.exe LibMCEnv.xml -bindings ..\Framework\HeadersDev -interfaces ..\Framework\InterfacesCore -suppresssubcomponents -suppresslicense -suppressstub -suppressexamples
act.exe LibMCPlugin.xml -bindings ..\Framework\HeadersCore -interfaces ..\Framework\InterfacesDev -suppresssubcomponents -suppresslicense -suppressstub -suppressexamples
act.exe LibMCDriver.xml -bindings ..\Framework\HeadersCore -interfaces ..\Framework\InterfacesDev -suppresssubcomponents -suppresslicense -suppressstub -suppressexamples
act.exe LibMCData.xml -bindings ..\Framework\HeadersCore -interfaces ..\Framework\InterfacesCore -suppresssubcomponents -suppresslicense -suppressstub -suppressexamples
act.exe LibMC.xml -bindings ..\Framework\HeadersCore -interfaces ..\Framework\InterfacesCore -suppresssubcomponents -suppresslicense -suppressstub -suppressexamples


pause