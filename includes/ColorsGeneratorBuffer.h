#ifndef COLOR_GENERATOR_BUFFER_H
#define COLOR_GENERATOR_BUFFER_H

#include "ColorsGenerator.h"

namespace colors
{
    // ColorGeneratorBuffer observes a ColorGenerator for the GUI: if a new
    // generator is created (changing for example from a ConstantColor to a
    // LinearGradient), it will 'notify()' all Observers. It will NOT notify if
    // the generator is modified however.
    class ColorGeneratorBuffer : public Observable
    {
    public:
        // Construct this object with a white ConstantColor.
        ColorGeneratorBuffer();

        // Construct initializing 'generator_' with 'gen'
        explicit ColorGeneratorBuffer(std::shared_ptr<ColorGenerator> gen);

        // Getter
        std::shared_ptr<ColorGenerator> get_generator() const;

        // Setter (notify)
        void set_generator(std::shared_ptr<ColorGenerator> gen);

    private:
        // The ColorGenerator. Shared with all Observers of 'generator_'
        // (VertexPainter for example).
        std::shared_ptr<ColorGenerator> generator_;
    };
}


#endif // COLOR_GENERATOR_BUFFER_H
