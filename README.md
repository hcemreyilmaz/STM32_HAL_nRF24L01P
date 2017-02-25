# #[PICProje](http://picproje.org/)

##### [Örnekler'e git](#Örnekler)
##### [Örnek CubeMX Konfigurasyonu'na git](#Örnek-cubemx-konfigurasyonu)

##### Kütüphaneyi hızlıca kullanmaya başlamak için aşağıdaki uyarıları dikkate almalısınız.

Bu kütüphane CubeMX yardımcı programı ile oluşturulacak projelere 
hızlıca entegre edilebilecek şekilde optimize edilmiştir. 

Projenizi CubeMX'te oluştururken bir adet SPI birimini, Full-Duplex modda, 
Hardware nSS desteği olmaksızın, en fazla 10Mbit hızında olacak şekilde aktif etmelisiniz. Buna ek olarak iki adet GPIO pinini mümkün olan en yüksek hızı seçerek Output Push-Pull modda aktif etmelisiniz. 

Bu pinlerden birinin kullanıcı tanımlı etiketi (User Label) xxx_CE, diğerinin ise xxx_nSS olmalı. 

**Not: Etiketlerin bir önemi yok ancak karışıklık yaratmamak adına _nSS ve _CE eklemelerini yapınız.**

Ayrıca bir adet GPIO_EXTIx (External Interrupt) pini aktif etmelisiniz. 

Önemli Not: CubeMX Code Generation safhasında "Generate peripheral initialization as a pair of '.c/.h' files per peripheral" seçeneği aktif olmalı.

# Örnek CubeMX Konfigurasyonu:
## GPIO
#### xxx_CE ve xxx_nSS için;
	-GPIO Output Level	    : High
	-GPIO Mode		        : Output Push Pull
	-GPIO Pull-up/Pull-down	: No pull-up and no pull-down
	-Maximum output speed	: Very High (veya mümkün olan en yüksek hız)
	-User Label		        : nRF24L01P_CE (ve _nSS)

#### -GPIO_EXTIx için;
	-GPIO Mode		        : External Interrupt Mode with Rising edge...
	-GPIO Pull-up/Pull-down	: No pull-up and no pull-down
	-User Label		        : Don't Matter...

## NVIC
	-EXTI xxx [xx xx]	    : Enabled ? Yes.

## SPIx
	-Frame Format	        : Motorola
	-Data Size		        : 8 Bits
	-First Bit		        : MSB First
	
	-Prescaler		        : 256 (10MBit'i geçmeyecek herhangi bir değer.)
	-BaudRate*		        : 175.781 KBits/s (Test amaçlı düşük hız)
	-Clock Polarity (CPOL)	: Low
	-Clock Phase (CPHA)	    : 1 Edge
	
	-CRC Calculation	    : Disabled
	-nSS Signal Type 	    : Software

# Örnekler
#### Basic TX RX Examples
	-Example 01		        : Basic TX & RX example *without* ACK.
				            : Used STM32F103C8T6 as Transmitter
				            : and STM32F429I-Disc1 as Receiver
				            : Activated UART Debugging for both of them.
	-Example 02	        	: Basic TX & RX example *with* ACK.
				            : Used STM32F103C8T6 as Transmitter
                            : and STM32F429I-Disc1 as Receiver
                            : Activated UART Debugging for both of them.
