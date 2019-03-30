#ifndef COLOR_GENERATOR_WRAPPER_H
#define COLOR_GENERATOR_WRAPPER_H

#include "Observer.h"
#include "ColorsGenerator.h"

namespace colors
{
    // ColorGeneratorWrapper observes a ColorGenerator for the GUI: if a new
    // generator is created (changing for example from a ConstantColor to a
    // LinearGradient), it will 'notify()' all Observers. It will also notify if
    // the generator is modified.
    class ColorGeneratorWrapper : public Observable
                               , public Observer<ColorGenerator>
    {
    public:
        using OGen = Observer<ColorGenerator>;
        
        // Construct this object with a white ConstantColor.
        ColorGeneratorWrapper();
        // Construct initializing 'generator_' with 'gen'
        explicit ColorGeneratorWrapper(std::shared_ptr<ColorGenerator> gen);
        // Rule-of-five shallow copy.
        ColorGeneratorWrapper(const ColorGeneratorWrapper& other);
        ColorGeneratorWrapper(ColorGeneratorWrapper&& other);
        ColorGeneratorWrapper& operator=(const ColorGeneratorWrapper& other);
        ColorGeneratorWrapper& operator=(ColorGeneratorWrapper&& other);

        ColorGeneratorWrapper clone() const;
        
        // Getter
        std::shared_ptr<ColorGenerator> unwrap() const;

        // Setter (notify)
        void wrap(std::shared_ptr<ColorGenerator> gen);
    };
}


#endif // COLOR_GENERATOR_WRAPPER_H
