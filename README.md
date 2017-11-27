![](./Resources/img/git-banner.jpg)

# About

Sinner _(pronounced SAH-IH-NER)_ is a sine wave synthesizer designed to showcase Linear Interpolation 
using JUCE and C++. There's also a neat metallic effect discovered during development.
 
## How to run:

Sinner has been tested on Windows 10 and Mac OS X El Capitan. The primary development has been done in OS X. Linux is supported, but not tested as of 26th November 2017. You can run the makefile to compile it.

To run the app, either use the .exe or .app provided (in the  Sinner/Builds/VisualStudio2017/x64/Debug/App or Sinner/Builds/MacOSX/build/Debug directories) or compile it yourself, using the directives below

## Compiling on Windows:

You'll need Microsoft Visual Studio 2015 or later. 
If you do not have it, install it and navigate to the Sinner/Builds/VisualStudio2017 directory and open the Sinner.sln file with Visual Studio. Install any required dependencies and then, on the toolbar at the top of the screen, under Build, hit Compile (CTRL + F7). 

This will create an Newtonium.exe executable in the Sinner/Builds/VisualStudio2017/x64/Debug/App directory
_(note that it's called Newtonium because of a name change mid-development)_
Just run it and there you have it.


## Compiling on OS X:

For OS X users, Xcode 8.2.1 or later is required. The latest version is recommended.

Navigate to the Sinner/Builds/MacOSX, and open the Sinner.xcodeproj in Xcode. After that, on the top left corner of the screen, press the play button and you'll compile an executable to Sinner/Builds/MacOSX/build/Debug, named 
Newtonium.app, and that's it.


## How to use:

Make sure you adjust your volume accordingly! The frequencies available range from negative values to ultrasound. 
Do not use the Effect and Interpolate buttons together. Doing so will just reset their stage to untoggled.


_(Portuguese version)_


# Sobre

Sinner _(pronunciado SAH-IH-NER)_ é um sintetizador de ondas seno criado para demonstrar Interpolação Linear, usando
JUCE e C++. Também tem um efeito interessante metálico descoberto durante o desenvolvimento.
 
## Como rodar:

Sinner foi testado no Windows 10 e em Mac OS X El Capitan. O desenvolvimento principal foi feito em OS X. Linux é suportado, só não testado. Basta rodar o makefile, e instalar as dependências .

Para rodar o app, use o .exe ou .app (providenciados na pasta Sinner/Builds/VisualStudio2017/x64/Debug/App ou Sinner/Builds/MacOSX/build/Debug) ou compile usando as instruções na pasta abaixo



## Compilação no Windows:

Você precisará do Microsoft Visual Studio 2015 ou mais recente. Recomenda-se o de 2017.
Se você não possui, instale e navegue para o diretório Sinner/Builds/VisualStudio2017 e abre o Sinner.sln com o VisualStudio2017. Instale as dependências que forem promptads na tela, e na toolbar, em Build, rode Compile. (CTRL+F7)

Isso criará um executável Newtonium.exe em Sinner/Builds/VisualStudio2017/x64/Debug/App _(obs: consta como newtonium.exe por conta de uma mudança de nome no meio do desenvolvimento)_.

Basta rodá-lo e pronto.

## Compilando no OS X:

Para usuários OS X, Xcode 8.2.1 ou superior é requerido e recomendado.

Navegue para o diretório Sinner/Builds/MacOSX, e abra o Sinner.xcodeproj no Xcode. Após isso, no canto superior esquerdo da tela, aperte o botão play de compilar e você compilará um executável para Sinner/Builds/MacOSX/build/Debug, denominado Newtonium.app. Basta rodá-lo. 
Navigate to the Sinner/Builds/MacOSX, and open the Sinner.xcodeproj in Xcode. After that, on the top left corner of the screen, press the play button and you'll compile an executable to Sinner/Builds/MacOSX/build/Debug, named 
Newtonium.app, and that's it.


## Como usar:

Ajuste o volume de acordo! Esse programa roda frequências em ultrasom que podem danificar seus ouvidos em volumes muito altos.

Não use o botão Interpolate junto com o Effect. Isso resetará os botões.




