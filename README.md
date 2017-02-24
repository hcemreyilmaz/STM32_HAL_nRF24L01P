### #PICProje.org
##### Kütüphaneyi hýzlýca kullanmaya baþlamak için aþaðýdaki uyarýlarý dikkate almalýsýnýz.

Bu kütüphane CubeMX yardýmcý programý ile oluþturulacak projelere 
hýzlýca entegre edilebilecek þekilde optimize edilmiþtir. 

Projenizi CubeMX'te oluþtururken bir adet SPI birimini, Full-Duplex modda, 
Hardware nSS desteði olmaksýzýn, en fazla 10Mbit hýzýnda olacak þekilde aktif etmelisiniz. Buna ek olarak iki adet GPIO pinini mümkün olan en yüksek hýzý seçerek Output Push-Pull modda aktif etmelisiniz. 

Bu pinlerden birinin kullanýcý tanýmlý etiketi (User Label) xxx_CE, diðerinin ise xxx_nSS olmalý. 

**Not: Etiketlerin bir önemi yok ancak karýþýklýk yaratmamak adýna _nSS ve _CE eklemelerini yapýnýz.**

Ayrýca bir adet GPIO_EXTIx (External Interrupt) pini aktif etmelisiniz. 

Önemli Not: CubeMX Code Generation safhasýnda "Generate peripheral initialization as a pair of '.c/.h' files per peripheral" seçeneði aktif olmalý.

# Örnek CubeMX Konfigurasyonu:
## GPIO
#### xxx_CE ve xxx_nSS için;
    -GPIO Output Level	    : High
    -GPIO Mode		        : Output Push Pull
    -GPIO Pull-up/Pull-down	: No pull-up and no pull-down
    -Maximum output speed	: Very High (veya mümkün olan en yüksek hýz)
    -User Label		        : nRF24L01P_CE (ve _nSS)

#### -GPIO_EXTIx için;
    -GPIO Mode		        : External Interrupt Mode with Rising edge...
    -GPIO Pull-up/Pull-down	: No pull-up and no pull-down
    -User Label		        : Don't Matter...

## NVIC
    -EXTI xxx [xx xx]       : Enabled ? Yes.

## SPIx
    -Frame Format	        : Motorola
    -Data Size		        : 8 Bits
    -First Bit              : MSB First
	
	-Prescaler		        : 256 (10MBit'i geçmeyecek herhangi bir deðer.)
    -BaudRate*	            : 175.781 KBits/s (Test amaçlý düþük hýz)
	-Clock Polarity (CPOL)	: Low
	-Clock Phase (CPHA)	    : 1 Edge
	
	-CRC Calculation	    : Disabled
	-nSS Signal Type	    : Software