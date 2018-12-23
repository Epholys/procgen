#ifndef VERTEX_PAINTER_COMPOSITE_H
#define VERTEX_PAINTER_COMPOSITE_H


#include <list>
#include "VertexPainterBuffer.h"

namespace colors
{
    class VertexPainterComposite;
    
    namespace impl
    {
        class ColorGeneratorComposite : public ColorGenerator
        {
        public:
            explicit ColorGeneratorComposite(VertexPainterComposite& painter);
            
            sf::Color get(float f) override;

            void reset_index();
            
        private:
            std::shared_ptr<ColorGenerator> clone_impl() const override;

            VertexPainterComposite& painter_;
            std::size_t index_;
        };

        class VertexPainterBufferObserver : public Observer<VertexPainterBuffer>
        {
        public:
            using OBuffer = Observer<VertexPainterBuffer>;
            
            explicit VertexPainterBufferObserver(std::shared_ptr<VertexPainterBuffer> painter_buffer,
                                                 VertexPainterComposite& painter_composite);
            // Shallow rule-of-five constructors.
            VertexPainterBufferObserver(const VertexPainterBufferObserver& other);
            VertexPainterBufferObserver(VertexPainterBufferObserver&& other);
            VertexPainterBufferObserver& operator=(const VertexPainterBufferObserver& other);
            VertexPainterBufferObserver& operator=(VertexPainterBufferObserver&& other);

            std::shared_ptr<VertexPainterBuffer> get_painter_buffer() const;
        private:
            VertexPainterComposite& painter_;
        };
    }
    
    class VertexPainterComposite : public VertexPainter
    {
    public:
        VertexPainterComposite(); // Create a default generator
        explicit VertexPainterComposite(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterComposite(const VertexPainterComposite& other);
        VertexPainterComposite(VertexPainterComposite&& other);
        VertexPainterComposite& operator=(const VertexPainterComposite& other);
        VertexPainterComposite& operator=(VertexPainterComposite&& other);

        std::list<std::shared_ptr<VertexPainterBuffer>> get_child_painters() const;
        std::shared_ptr<VertexPainterBuffer> get_main_painter() const;
                
        void set_child_painters(const std::list<std::shared_ptr<VertexPainterBuffer>> painters);
       
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) override;

        static bool has_copied_painter();
        static std::shared_ptr<VertexPainter> get_copied_painter();
        static void save_painter(std::shared_ptr<VertexPainter> painter);

    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;

        static std::shared_ptr<VertexPainter> copied_painter_;

        friend impl::ColorGeneratorComposite;
        std::shared_ptr<impl::ColorGeneratorComposite> color_distributor_;        
        impl::VertexPainterBufferObserver main_painter_;

        std::list<std::vector<std::size_t>> vertices_index_groups_;
        friend impl::VertexPainterBufferObserver;
        std::list<impl::VertexPainterBufferObserver> child_painters_;
    };
}

#endif // VERTEX_PAINTER_COMPOSITE_H
