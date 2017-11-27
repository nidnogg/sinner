#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <limits>


//=======================================================================================================================
/*
                        Projeto Final de Cálculo Numérico
                                    Sinner
                        Autor: Henrique Vermelho de Toledo
                        DRE: 116076549
                        

    Este arquivo, entitulado originalmente MainComponent.cpp, 
    é o responsável pelo processamento de áudio principal do Sinner.
    Os padrões gerais de código -> indentação no estilo Allman, tabbeamento em 4 espaços, 
    espaço antes de parêntesis preenchido, nomes de variáveis em camelCase foram todos
    seguidos para máxima consistência e leitura do código.

    Um guia geral sobre seu funcionamento:
        O principal aqui é a classe MainContentComponent, que herda as classes AudioAppComponent, 
        Slider::Listener e TextButton::Listener.
        A primeira classe dessas herdadas, AudioAppComponent, traz os principais métodos usados no processamento
        prepareToPlay(), releaseResources(), getNextAudioBlock(), shutdownAudio(), dentre outras.
        
        As outras duas, Slider::Listener e TextButton::Listener trazem callbacks que permitem interagir com os sliders
        e os botões encontrados no aplicativo.

        Cada um explicado devidamente abaixo.

        O grosso do processamento de interpolação está definido dentro da função getNextAudioBlock().
 */
//=======================================================================================================================

class MainContentComponent : public AudioAppComponent, public Slider::Listener, public TextButton::Listener
{
public:
    // Construtor de MainContentComponent 
    MainContentComponent()

    {
        // Dimensões
        setSize (600, 250);
        
        // Usaremos dois canais - stereo
        setAudioChannels (0, 2);
        
        
        // Slider de volume
        addAndMakeVisible (volumeSlider);                     // Cria e mostra na tela com parâmetros do método resized()
        volumeSlider.setRange (-96, 6);                       // Alcance do Slider
        volumeSlider.setTextValueSuffix (" db");              // Sufixo do valor, nesse caso, decibéis
        volumeSlider.setValue (-6);                           // Valor padrão
        volumeSlider.addListener (this);                      // Adiciona listener para que funções callback saibam que ele existe
        volumeSlider.setSkewFactorFromMidPoint(0.5);          // Esse é o valor que o meio do slider representa
        volumeLabel.setText ("Volume", dontSendNotification); // Texto da etiqueta
        volumeLabel.attachToComponent (&volumeSlider, true);  // Junta a etiqueta ao slider. True é onLeft, ou seja, à esquerda do slider
        
        // Slider de frequência - esse é o que será interpolado.
        /*
        O valor para o qual esse slider apontar será a frequência alvo (targetFrequency) que iremos usar na interpolação.
        Serão encontrados pontos entre o valor atual da frequência e a frequência alvo pontos entre a função 
        */
        addAndMakeVisible (freqSlider);
        freqSlider.setRange (10, 20000);            // Os atributos desse slider são definidos de maneira análoga ao de cima
        freqSlider.setTextValueSuffix (" Hz");       
        freqSlider.setValue (500.0);
        freqSlider.addListener (this);
        freqSlider.setSkewFactorFromMidPoint (500);
        freqLabel.setText ("Freq", dontSendNotification);
        freqLabel.attachToComponent (&freqSlider, true);
        
        // Mute Button
        addAndMakeVisible (muteButton);                     // Cria e mostra na tela com parâmetros do método resized()
        muteButton.setButtonText ("Mute");                  // Texto do botão
        muteButton.addListener (this);                      // Listener para Callbacks 
        muteButton.setEnabled (true);                       // Comportamento padrão
        
        // Interpolate Button
        addAndMakeVisible (interpolateButton);              //Esse botão toggleia a interpolação. O efeito é uma suavização na onda emitida
        interpolateButton.setButtonText ("Interpolate");
        interpolateButton.addListener (this);
        interpolateButton.setEnabled (true);
        
        // Effect Button                           // Esse botão foi um efeito surpresa descoberto durante a implementação do método!
        addAndMakeVisible (effectButton);         // Use com volume baixo! :)        
        effectButton.setButtonText ("Mystery Effect (Make sure to lower the volume!)");
        effectButton.addListener (this);
        effectButton.setEnabled (true);
        
        setWantsKeyboardFocus (true);             // Isso permite que a funçao keyPressed() use o teclado para as teclas do sintetizador
        
        
    }
    // Destrutor, desmonta o áudio quando fecha o arquivo
    ~MainContentComponent()
    {
        shutdownAudio();
    }
    // Essa função verifica que teclas estão sendo pressionadas
    bool keyPressed (const KeyPress &k) override
    {
        /*
        Como a frequência está diretamente atrelada ao tom, cada nota em 
        cada instrumento musical possui um valor definido em hertz.

        Tirando proveito disso, surge essa implementação aonde a seguir 
        são definidas as frequências para a oitava do dó maior (middle C = C4) de um piano normal
        seguindo o padrão encontrado no site Sengpiel Audio (ver referências - relatório)

        É uma implementação grosseira de um teclado virtual, mas como sequer estamos 
        usando MIDI (o padrão mais escalável no mercado para representar notas em computadores)
        não precisamos de mais que isso :)

        */

        float c4 = 261.62, c4s = 277.18, d4 = 293.66, d4s = 311.12, e4 = 329.62;
        float f4 = 349.22, f4s = 369.99, g4 = 391.99, g4s = 415.30;
        float a4 = 440.00, a4s =  466.16, b4 = 493.88, c5 = 523.25;
    
        bool keyPress;
        
        switch (k.getTextCharacter()) // k.getTextCharacter() retorna a tecla pressionada
        {
            case 'a' : freqSlider.setValue (c4);
                       break;
            case 's' : freqSlider.setValue (d4);
                       break;
            case 'd' : freqSlider.setValue (e4);
                       break;
            case 'w' : freqSlider.setValue (c4s);
                       break;
            case 'e' : freqSlider.setValue (d4s);
                       break;
            case 'f' : freqSlider.setValue (f4);
                       break;
            case 't' : freqSlider.setValue (f4s);
                       break;
            case 'g' : freqSlider.setValue (g4);
                       break;
            case 'y' : freqSlider.setValue (g4s);
                       break;
            case 'h' : freqSlider.setValue (a4);
                       break;
            case 'u' : freqSlider.setValue (a4s);
                       break;
            case 'j' : freqSlider.setValue (b4);
                       break;
            case 'k' : freqSlider.setValue (c5);
                       break;
            default :  freqSlider.setValue (0.0); // Qualquer tecla não reconhecida retorna o tom para 0.0
        
        }
        return keyPress;
    }

    // Callback para quando o botão é clicado. Não funciona sem chamar a addListener() descrita no construtor de MainContentComponent
    void buttonClicked (Button* button) override
    {
        // Temos três botões na tela, um de mute, interpolação e um de efeitos. Nos ifs, são usados os seus endereços
        if (button == &muteButton)
        {
            mute = !mute;       // Variável booleana para representar estado mute
            std::cout << "Mute: " << mute << std::endl; // Printa 1 no console para estado ativo, 0 para inativo

        }
        if (button == &interpolateButton)
        {
            interpolating = !interpolating; // Similar à cima
            std::cout << "Interpolating: " << interpolating << std::endl;

        }
        if (button == &effectButton)
        {
            effect = !effect;  //Similar à cima
            std::cout << "Effect: " << effect << std::endl;
        }
        
    }
    

    // Callback para quando o valor do Slider é alterado. Não funciona sem chamar a addListener() descrita no construtor de MainContentComponent
    void sliderValueChanged (Slider *slider) override
    {
        if (slider == &volumeSlider)
        {
            // Amplitude é dada em decibéis - descrito no relatório
            targetAmplitude = pow (10, ((float)volumeSlider.getValue() / 20.0));

        }
        
        if (slider == &freqSlider)
        {
            if (!interpolating )
            {
                targetFrequency = (float) freqSlider.getValue(); //Pega valor do slider
            }
            else
            {
                targetFrequency = (float) freqSlider.getValue();
            }
         
        }
    
      
    }
    
    /*
    Essa função é chamada antes da saída de áudio iniciar, ou quando altera-se a sample rate | buffer size.

    Lembrando um pouco do funcionamento base descrito no relatório, para sintetizar o som, ele é convertido para amostras
    discretas (samples) carregadas em buffers de um tamanho determinado (o block size) que serão propagados nos canais 
    de áudio do computador.

    A sample rate é a taxa de amostragem, ou seja, quantas samples serão emitidas por segundo.
    O block size, medido em bits, dita o tamanho do buffer de amostras que iremos preencher 
    com amostras processadas. Por exemplo, se temos um buffer size hipotético de 441 bits,
    e uma sample rate de 44100hz (padrão CD-ROM, usada no Sinner)
    caberão 441 samples em um buffer. Logo, a função de preencher o buffer com samples 
    será chamada 100 vezes por segundo!
    */
   
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {     
        amplitude = 0.5;                                        // Amplitude inicial da onda seno 
        targetAmplitude = amplitude;                            // Amplitude alvo (oriunda do slider)
        frequency = 500.0;                                      // Frequencia inicial
        targetFrequency = frequency;                            // Frequênca alvo (oriunda do slider) - destino da interpolação
        time = 0.0;                                             // Tempo medido em millisegundos
        timeDelta = 1 / sampleRate;                             // Tempo entre cada sample, usado como contador | incremento
        currentSampleRate = sampleRate;                         // SampleRate atual
        interpolationLength = currentSampleRate * 0.100;            
        angle  = 0.0;                                           // Ângulo inicial. Ele será atualizado para cada amostra.
        
        /*
        f.open ("data.txt", ios::out);
        if (f.is_open()) cout << "File succesfully opened!\n";
        */
    }
    /*
    Nessa função ocorre o processamento de áudio. Ela recebe um buffer que será preenchido por samples,
    que nesse caso é de tamanho 512 bits, e será enviado  A cada segundo, essa função será chamada cerca de 86 (512/44100) vezes para
    preencher a samplerate de 44100 samples por segundo!  
    
    Nela, usa-se um padrão comum de programação de Digital Signal Processing (DSP):
    Evitamos condicionais dentro dos for loops que lidam com as samples, e prezamos for loops separados para cada tipo de processamento diferente.
    
    Sobre a estrutura da função: 
    Há três condicionais if, um para a implementação ingênua sem interpolação, aonde usamos
    como output a função seno definida no relatório como abordagem ingênua,
    e outros para os dois botões Interpolate e Mystery Effect.
    Não podem ser ligados esses dois ao mesmo tempo - nesse caso é printado no console uma mensagem de erro e 
    valores são resetados.

    O modo interpolado faz uso de uma função externa - updateAngle(), que atualiza o incremento que o ângulo deve ter
    entre cada sample.
    Além disso, interpolamos a frequência de modo que a transição de valores no slider seja contínua, e não discreta,
    o que causaria artefatos demonstrados na abordagem ingênua. Criamos incrementos pequenos da frequência, logo antes do 
    loop para as samples, que serão somados continuamente para formar uma transição gradual entre os valores.

    
    */
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
       // Na contagem de tempo, caso o tempo chegue nos limites numéricos, resetamos o mesmo
        if (time >= std::numeric_limits<float>::max())
        {
            time = 0.0;
        }

        const double localTargetFrequency = targetFrequency; // Cópia local da frequência alvo, caso o slider mude enquanto
                                                            // está sendo interpolado o valor
        double sampleFail = 0.0;    // sampleFail é a sample ingênua, que é a aplicação regular de uma onda seno amplit* seno(2*pi*freq); 
        /*
        Pegamos o ponteiro para um buffer em mono (1 canal), e depois de processá-lo,
        jogamos ele num for loop secundário para cada um dos dois canais de saída que definimos anteriormente,
        resultando num funcionamento em stereo
        */
        float *monoBuffer = new float[bufferToFill.numSamples];
    
        if (mute) return bufferToFill.clearActiveBufferRegion(); // Se estiver em mute, limpamos o buffer
    
        if (!interpolating && !effect)
        {
            // for para processar as samples de um dado buffer
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample) // numSamples aqui é 512
            {
                // Fórmula ingênua de computar a onda seno a cada sample
                sampleFail = targetAmplitude * std::sin(2 * double_Pi * localTargetFrequency * time);
                monoBuffer[sample] = sampleFail; // Cada sample do buffer vira a sampleFail processada
                time += timeDelta;              // Somamos o tempo com o tempo entre samples para contar em ms
                
                //Diagnostics
                //printf ("regular sample: %f %f\n", sampleFail, time);
                //std::cout << "Does file exist lel" << f.exists() << std::endl;
                //printf("frequency %f\n", localTargetFrequency);
                //printf("amplitude %f\n", targetAmplitude);
            }
        }
        // Note o && - não pode ser usado a interpolação com o efeito ligado
        else if (interpolating && (effect == false))
        {
            // Se a frequência do slider é a atual, não precisamos interpolar nada!
            if (localTargetFrequency == frequency)
            {

                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    const float curSample = (float) std::sin (angle);
                    angle += angleDelta;  // Apenas variamos o ângulo a cada iteração
                    monoBuffer[sample] = targetAmplitude * curSample; // Amplitude 
                    updateAngle();
                    time += timeDelta;
                    
                    //Diagnostics
                    // printf ("non-moving sample: (%f %f)\n", curSample, time);
                    // printf("frequency interpolating %f\n", frequency);
                    // printf("amplitude interpolating %f\n", targetAmplitude);
                }
            }
            /* 
            Aqui ocorre a interpolação. O uso do frequencyDelta e sua soma contínua à frequência, 
            cria pontos (frequências) novos entre a frequência atual e a frequência alvo, interpolando
             a transição de valor.
            */
            else
            {
               // O frequencyDelta é uma variação pequena que será incrementada continuamente à frequência
               // para que formemos a transição suave anteriormente discutida
                const double frequencyDelta = (localTargetFrequency - frequency) / bufferToFill.numSamples; 
                // Note acima que o incremento é feito baseado no número de samples do buffer (512), logo, interpolamos no intervalo de
                // 512 passos e para buffer sizes pequenos (256, 128) ainda poderíamos ouvir artefatos. Não nesse caso.
                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    const float curSample = (float) std::sin (angle);
                    frequency += frequencyDelta; // Cada sample das 512 é incrementada aos poucos, de modo que não hajam mudanças abrutas
                    updateAngle();          // Atualizamos o ângulo na função externa     
                    angle += angleDelta;         // Somamos o ângulo com seu incremento
                    monoBuffer[sample] = targetAmplitude * curSample; // A sample é então multiplicada pelo volume (amplitude) e armazenada no buffer.
                    time += timeDelta;           // Tempo contado como antes
                    
                    //Diagnostics                    
                    // printf ("moving sample: (%f %f)\n", curSample, time);
                    // printf("frequency interpolating %f\n", frequency);
                    // printf("amplitude interpolating %f\n", targetAmplitude);
                    
                }
                // Após a transição, a frequência antiga passa a ser a nova.
                frequency = localTargetFrequency;
            }
        }
        /*
        Aqui, antes de se pensar na função externa, uma tentativa de interpolar resultou num efeito interessante e é 
        deixado aqui para fins de curiosidade.
        */
        else if (effect && (interpolating == false)) // Funcionamento análogo a anterior
        {
            if (localTargetFrequency == frequency)
            {
                double angle =  2 * double_Pi * (frequency / currentSampleRate);  // Ângulo é atualizado aqui ao invés de na updateAngle()
                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    float curSample = (float) std::sin (angle);       // Computa seno para o angulo
                    float sampleDelta = 2.0 * double_Pi * (frequency / currentSampleRate);  // Incremento do ângulo
                    angle += sampleDelta;       
                    monoBuffer[sample] = curSample;
                    time += timeDelta;
                    
                }
            }
            else  //"Interpolação"
            {

                double frequencyDelta = (localTargetFrequency - frequency) / bufferToFill.numSamples;
                double angle =  2 * double_Pi * (frequency / currentSampleRate);
                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    // O que segue abaixo é praticamente o mesmo código do que na interpolação acima - só que 
                    // aparentemente, ao atualizar o ângulo fora do loop e não numa função externa, modulamos a onda
                    // amplificado som amplificado e extremamente metálico, algo legal de se explorar numa suite de efeitos.
                    float curSample = (float) std::sin (angle);
                    frequency += frequencyDelta;
                    float sampleDelta = 2.0 * double_Pi * (frequency / currentSampleRate);
                    angle += sampleDelta;
                    monoBuffer[sample] = targetAmplitude * curSample;
                    time += timeDelta;                    
                }
            }
        }
        else
        {
            // Printa no console um erro caso interpolação e effect estejam ativados juntos e reseta eles
            printf("Error! Setting buttons to defaults.\n");
            interpolating = false;
            effect = false;
            
        }
        // Itera sobre todos os canais de output
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            // Aqui pegamos um ponteiro para a sample inicial no buffer para esse canal de saída
            float* const buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
            
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                // Alimentamos o buffer com as samples processadas no monoBuffer[]
                buffer[sample] = monoBuffer[sample];
 
            }
        }
     
    }
    
    // Essa é a função updateAngle() que atualiza o valor da ângulo computado para a função seno
    void updateAngle()
    {
        // Pegamos quantos ciclos há em uma sample - dividindo a frequência pela sample rate (44100hz)
        const double cyclesPerSample = freqSlider.getValue() / currentSampleRate;
        angleDelta = cyclesPerSample * 2.0 * double_Pi; 
    }

    //Essa função seria usada para liberar recursos em certos casos, mas no Sinner já basta 
    //o destrutor do MainContentComponent desligar o áudio.
    void releaseResources() override
    {
      
    }
    

    // Desenhos na tela extras
    void paint(Graphics& g) override
    {
        // Preenche o fundo com a cor dark grey
        g.fillAll(Colours::darkgrey);
    }
    /*
    Essa função em tese é chamada quando o componente é resizeado, mas há um comportamento muito útil também
    que permite definir o tamanho e a posição dos elementos criados. (Sliders, botões, texto, etc)
    */
    void resized() override
    {
        const int sliderLeft = 50;
        volumeSlider.setBounds (sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
        freqSlider.setBounds (sliderLeft, 50, getWidth() - sliderLeft - 10, 20);
        muteButton.setBounds (10, 110, getWidth() - 20, 20);
        interpolateButton.setBounds (10, 150, getWidth() - 20, 20);
        effectButton.setBounds (10, 190, getWidth() - 20, 20);
    }
    
private:    
    // Variáveis vistas no código acima e já descritas
    float currentSampleRate;
    float amplitude, targetAmplitude;
    float frequency, targetFrequency;
    float time;
    float timeDelta;
    float interpolationLength;
    float frac;
    float angleDelta, angle;
    // Interface Gráfica (GUI)
    Slider volumeSlider;
    Slider freqSlider;
    Label volumeLabel;
    Label freqLabel;
    
    TextButton muteButton;
    TextButton interpolateButton;
    TextButton effectButton;
    bool mute;
    bool interpolating;
    bool effect;
    
    //Código padrão gerado para otimização
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


// Essa função é chamada no código de início do app para criar o MainComponent.
Component* createMainContentComponent() { return new MainContentComponent(); }

#endif  // MAINCOMPONENT_H_INCLUDED

