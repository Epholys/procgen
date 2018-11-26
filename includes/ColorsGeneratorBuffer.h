#ifndef COLOR_GENERATOR_BUFFER_H
#define COLOR_GENERATOR_BUFFER_H

#include "Observer.h"
#include "ColorsGenerator.h"

namespace colors
{
    // ColorGeneratorBuffer observes a ColorGenerator for the GUI: if a new
    // generator is created (changing for example from a ConstantColor to a
    // LinearGradient), it will 'notify()' all Observers. It will also notify if
    // the generator is modified.
    class ColorGeneratorBuffer : public Observable
                               , public Observer<ColorGenerator>
    {
    public:
        using OGen = Observer<ColorGenerator>;
        
        // Construct this object with a white ConstantColor.
        ColorGeneratorBuffer();
        // Construct initializing 'generator_' with 'gen'
        explicit ColorGeneratorBuffer(std::shared_ptr<ColorGenerator> gen);
        // Rule-of-five shallow copy.
        ColorGeneratorBuffer(const ColorGeneratorBuffer& other);
        ColorGeneratorBuffer(ColorGeneratorBuffer&& other);
        ColorGeneratorBuffer& operator=(const ColorGeneratorBuffer& other);
        ColorGeneratorBuffer& operator=(ColorGeneratorBuffer&& other);

        // Getter
        std::shared_ptr<ColorGenerator> get_generator() const;

        // Setter (notify)
        void set_generator(std::shared_ptr<ColorGenerator> gen);
    };
}


#endif // COLOR_GENERATOR_BUFFER_H
