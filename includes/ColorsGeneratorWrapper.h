#ifndef COLOR_GENERATOR_WRAPPER_H
#define COLOR_GENERATOR_WRAPPER_H

#include "ColorsGenerator.h"

namespace colors
{
    // ColorGeneratorWrapper observes a ColorGenerator for the GUI: if a new
    // generator is created (changing for example from a ConstantColor to a
    // LinearGradient), it will 'notify()' all Observers. It will also notify if
    // the generator is modified.
    class ColorGeneratorWrapper : public Indicator
    {
    public:
        // Construct this object with a white ConstantColor.
        ColorGeneratorWrapper();
        virtual ~ColorGeneratorWrapper() {};
        // Construct initializing 'generator_' with 'gen'
        explicit ColorGeneratorWrapper(std::shared_ptr<ColorGenerator> gen);
        // Rule-of-five depp copy.
        ColorGeneratorWrapper(const ColorGeneratorWrapper& other);
        ColorGeneratorWrapper(ColorGeneratorWrapper&& other);
        ColorGeneratorWrapper& operator=(const ColorGeneratorWrapper& other);
        ColorGeneratorWrapper& operator=(ColorGeneratorWrapper&& other);

        // Getter
        std::shared_ptr<ColorGenerator> unwrap() const;

        // Setter (notify)
        void wrap(std::shared_ptr<ColorGenerator> gen);

        virtual bool poll_modification() override;

    private:
        std::shared_ptr<ColorGenerator> generator_;
    };
}


#endif // COLOR_GENERATOR_WRAPPER_H
