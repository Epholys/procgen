#ifndef VERTEX_PAINTER_COMPOSITE_H
#define VERTEX_PAINTER_COMPOSITE_H


#include <vector>

#include "types.h"
#include "VertexPainterWrapper.h"

namespace colors
{
    class ColorGeneratorSerializer;

    // The VertexPainterComposite is a class allowing to compose other
    // VertexPainters. For example, for a VertexPainterLinear, one side of the
    // LSystemView could be managed with a VertexPainterRadial and the other by
    // a VertexPainterRandom.
    // The implementation is quite complicated, as it itself inherits from the
    // more usual VertexPainter and must manage them with all the ColorGenerator
    // stuff.
    class VertexPainterComposite;

    // Implementation class only used by 'VertexPainterComposite'
    namespace impl
    {
        // A VertexPainterComposite manages a main painter, for example the
        // VertexPainterLinear mentionned before. This main painter however does
        // not paint anything in this case. We use 'ColorGeneratorComposite' to
        // put each vertex in a specific pool in 'VertexPainterComposite'
        // according to the number 'f' at each call of 'get()' Each pool
        // corresponds to a slave painter that will paint the vertices in its
        // own way.
        //
        // As such, this is an object exclusively created for the slave painter,
        // and it is managed by 'VertexPainterComposite' and linked to it by a
        // pointer.
        class ColorGeneratorComposite : public ColorGenerator
        {
        public:
            ColorGeneratorComposite();
            explicit ColorGeneratorComposite(VertexPainterComposite* painter);
            virtual ~ColorGeneratorComposite() {}
            ColorGeneratorComposite(const ColorGeneratorComposite&) = delete;
            ColorGeneratorComposite(ColorGeneratorComposite&&) = delete;
            ColorGeneratorComposite& operator=(const ColorGeneratorComposite&) = delete;
            ColorGeneratorComposite& operator=(ColorGeneratorComposite&&) = delete;

            // Returns a dummy sf::Color::Transparent but as a side-effect fills
            // 'vertex_indices_pools_' of VertexPainterComposite.
            //
            // Exceptions;
            //  - Precondition: 'painter_.vertex_indices_pools_' must not be empty.
            sf::Color get(float f) override;

            // Called by 'painter_' before painting, reset 'global_index_' to
            // 0.
            void reset_index();

            friend class ::colors::ColorGeneratorSerializer;
            virtual std::string type_name() const override;

        private:
            // Deep-copy cloning method.
            std::shared_ptr<ColorGenerator> clone() const override;

            // Pointer to the linked VertexPainterComposite.
            // Dangerous, but allows nullptr.
            VertexPainterComposite* painter_;

            // The global 'index_' of the vertex array. Incremented at each
            // 'get()' call.
            std::size_t global_index_;

            friend class cereal::access;
            template<class Archive>
            void save(Archive&, const u32) const
                {
                }
            template<class Archive>
            void load(Archive&, const u32)
                {
                }
        };
    }

    class VertexPainterSerializer;

    // The main class.
    // Also manages a painter which can be copied and pasted everywhere (as it
    // is static).
    class VertexPainterComposite : public VertexPainter
    {
    public:
        VertexPainterComposite(); // Create a default generator
        // Useless constructor: only calls the default constructor. Defined to
        // have a common interface with the other painters.
        explicit VertexPainterComposite(const ColorGeneratorWrapper&);
        virtual ~VertexPainterComposite() {}
        // This class is mainly used polymorphic-ally, so deleting these
        // constructors saved some LoC so potential bugs.
        VertexPainterComposite(const VertexPainterComposite& other) = delete;;
        VertexPainterComposite(VertexPainterComposite&& other) = delete;;
        VertexPainterComposite& operator=(const VertexPainterComposite& other) = delete;;
        VertexPainterComposite& operator=(VertexPainterComposite&& other) = delete;;

        // Getters/Setters
        const std::vector<VertexPainterWrapper>& get_child_painters() const;
        std::vector<VertexPainterWrapper>& ref_child_painters();
        const VertexPainterWrapper& get_main_painter() const;
        VertexPainterWrapper& ref_main_painter();

        void set_main_painter(const VertexPainterWrapper& painter_buff);
        void set_child_painters(const std::vector<VertexPainterWrapper>& painters);

        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<u8>& iteration_of_vertices,
                                    const std::vector<bool>& transparent,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) override;

        // Draw all the supplementary_drawing from the main and children painters.
        virtual void supplementary_drawing(sf::FloatRect bounding_box) const override;

        // Static methods to manage the copy of the VertexPainter.
        static bool has_copied_painter();
        static std::shared_ptr<VertexPainter> get_copied_painter();
        static void save_painter(std::shared_ptr<VertexPainter> painter);

        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone() const override;

        friend class VertexPainterSerializer;
        virtual std::string type_name() const override;

        virtual bool poll_modification() override;

    private:

        // The copied painter
        static std::shared_ptr<VertexPainter> copied_painter_;

        // Easy access for 'ColorGeneratorComposite'.
        friend impl::ColorGeneratorComposite;
        std::shared_ptr<impl::ColorGeneratorComposite> color_distributor_;

        // The main painter.
        VertexPainterWrapper main_painter_;

        // The pools for the indices of vertices.
        // Each pool corresponds to a painter in 'child_painters_observers_'. It
        // is filled by 'color_distributor' and contains the indices of the
        // vertices that will be painted by painter by the child painter. The
        // index i corresponds to the i-th vertex in the 'vertices' parameters
        // in 'paint_vertices()'.
        std::vector<std::vector<std::size_t>> vertex_indices_pools_;

        std::vector<VertexPainterWrapper> child_painters_;

        // Hack to avoid circular dependency between VertexPainterSerializer and
        // VertexPainterComposite. The VertexPainterSerializer is never included
        // in this file, but is used as a template parameter that will be
        // compiled later.
        template<class Archive, class Serializer>
        void save_impl(Archive& ar, const u32) const
            {
                static_assert(std::is_same<Serializer, VertexPainterSerializer>::value);

                auto main_painter = Serializer(main_painter_.unwrap());
                std::vector<Serializer> child_painters;
                for(const auto& child : child_painters_)
                {
                    child_painters.push_back(Serializer(child.unwrap()));
                }
                ar(cereal::make_nvp("main_painter", main_painter),
                   cereal::make_nvp("child_painters", child_painters));
            }
        template<class Archive, class Serializer>
        void load_impl(Archive& ar, const u32)
            {
                static_assert(std::is_same<Serializer, VertexPainterSerializer>::value);

                auto main_painter_serializer = Serializer();
                std::vector<Serializer> child_painters_serializers;
                ar(cereal::make_nvp("main_painter", main_painter_serializer),
                   cereal::make_nvp("child_painters", child_painters_serializers));

                set_main_painter(VertexPainterWrapper(main_painter_serializer.get_serialized()));

                std::vector<VertexPainterWrapper> child_wrappers;
                for(const auto& painter_serializer : child_painters_serializers)
                {
                    child_wrappers.push_back(VertexPainterWrapper(painter_serializer.get_serialized()));
                }
                set_child_painters(child_wrappers);

                // Little hack to avoid updating change at first creation
                poll_modification();
            }

        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const u32) const
            {
                u32 unused = 0;
                save_impl<Archive, VertexPainterSerializer>(ar, unused);
            }
        template<class Archive>
        void load(Archive& ar, const u32)
            {
                u32 unused = 0;
                load_impl<Archive, VertexPainterSerializer>(ar, unused);
            }
    };
}

#endif // VERTEX_PAINTER_COMPOSITE_H
