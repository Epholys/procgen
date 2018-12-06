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

       
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) override;

    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;

        std::vector<sf::Vertex> vertices_copy_;
        
        friend impl::ColorGeneratorComposite;
        std::shared_ptr<impl::ColorGeneratorComposite> color_distributor_;        
        std::shared_ptr<VertexPainterBuffer> main_painter_;

        std::list<std::vector<std::size_t>> vertices_index_groups_;
        std::list<std::shared_ptr<VertexPainterBuffer>> child_painters_;
    };
}

#endif // VERTEX_PAINTER_COMPOSITE_H
