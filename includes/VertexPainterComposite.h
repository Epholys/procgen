#ifndef VERTEX_PAINTER_COMPOSITE_H
#define VERTEX_PAINTER_COMPOSITE_H


#include <vector>
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
            void save(Archive&, const std::uint32_t) const
                {
                }
            template<class Archive>
            void load(Archive&, const std::uint32_t)
                {
                }
        };

        // A utility class to manipulate an Observer of a
        // 'VertexPainterWrapper'. Used in 'VertexPainterComposite' as the main
        // and slave painters. The rational is to call 'painter_.notify()' at
        // each modification of each painter.
        class VertexPainterWrapperObserver : public Observer<VertexPainterWrapper>
        {
        public:
            using OWrapper = Observer<VertexPainterWrapper>;

            VertexPainterWrapperObserver() = delete;
            VertexPainterWrapperObserver(std::shared_ptr<VertexPainterWrapper> painter_wrapper,
                                         VertexPainterComposite* painter_composite);
            VertexPainterWrapperObserver(const VertexPainterWrapperObserver& other);
            VertexPainterWrapperObserver(VertexPainterWrapperObserver&& other);
            VertexPainterWrapperObserver& operator=(const VertexPainterWrapperObserver& other);
            VertexPainterWrapperObserver& operator=(VertexPainterWrapperObserver&& other);

            std::shared_ptr<VertexPainterWrapper> get_painter_wrapper() const;
            void set_painter_wrapper(std::shared_ptr<VertexPainterWrapper> painter_buff);

            void set_composite_painter(VertexPainterComposite* painter);

        private:
            // Pointer to the linked VertexPainterComposite
            VertexPainterComposite* painter_;
        };
    }
}

namespace colors
{
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
        explicit VertexPainterComposite(const std::shared_ptr<ColorGeneratorWrapper>);
        virtual ~VertexPainterComposite() {}
        // This class is mainly used polymorphic-ally, so deleting these
        // constructors saved some LoC so potential bugs.
        VertexPainterComposite(const VertexPainterComposite& other) = delete;;
        VertexPainterComposite(VertexPainterComposite&& other) = delete;;
        VertexPainterComposite& operator=(const VertexPainterComposite& other) = delete;;
        VertexPainterComposite& operator=(VertexPainterComposite&& other) = delete;;

        // Getters/Setters
        std::vector<std::shared_ptr<VertexPainterWrapper>> get_child_painters() const;
        std::shared_ptr<VertexPainterWrapper> get_main_painter() const;
        void set_main_painter(std::shared_ptr<VertexPainterWrapper> painter_buff);
        void set_child_painters(const std::vector<std::shared_ptr<VertexPainterWrapper>> painters);

        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<std::uint8_t>& iteration_of_vertices,
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

    private:

        // The copied painter
        static std::shared_ptr<VertexPainter> copied_painter_;

        // Easy access for 'ColorGeneratorComposite'.
        friend impl::ColorGeneratorComposite;
        std::shared_ptr<impl::ColorGeneratorComposite> color_distributor_;

        // The main painter.
        friend impl::VertexPainterWrapperObserver;
        impl::VertexPainterWrapperObserver main_painter_observer_;

        // The pools for the indices of vertices.
        // Each pool corresponds to a painter in 'child_painters_observers_'. It
        // is filled by 'color_distributor' and contains the indices of the
        // vertices that will be painted by painter by the child painter. The
        // index i corresponds to the i-th vertex in the 'vertices' parameters
        // in 'paint_vertices()'.
        std::vector<std::vector<std::size_t>> vertex_indices_pools_;

        std::vector<impl::VertexPainterWrapperObserver> child_painters_observers_;

        // Hack to avoid circular dependency between VertexPainterSerializer and
        // VertexPainterComposite. The VertexPainterSerializer is never included
        // in this file, but is used as a template parameter that will be
        // compiled later.
        template<class Archive, class Serializer>
        void save_impl(Archive& ar, const std::uint32_t) const
            {
                static_assert(std::is_same<Serializer, VertexPainterSerializer>::value);

                auto main_painter = Serializer(main_painter_observer_.get_painter_wrapper()->unwrap());
                std::vector<Serializer> child_painters;
                for(const auto& child : child_painters_observers_)
                {
                    child_painters.push_back(Serializer(child.get_painter_wrapper()->unwrap()));
                }
                ar(cereal::make_nvp("main_painter", main_painter),
                   cereal::make_nvp("child_painters", child_painters));
            }
        template<class Archive, class Serializer>
        void load_impl(Archive& ar, const std::uint32_t)
            {
                static_assert(std::is_same<Serializer, VertexPainterSerializer>::value);

                auto main_painter_serializer = Serializer();
                std::vector<Serializer> child_painters_serializers;
                ar(cereal::make_nvp("main_painter", main_painter_serializer),
                   cereal::make_nvp("child_painters", child_painters_serializers));

                std::shared_ptr<VertexPainterWrapper> main_wrapper = std::make_shared<VertexPainterWrapper>();
                main_wrapper->wrap(main_painter_serializer.get_serialized());
                set_main_painter(main_wrapper);

                std::vector<std::shared_ptr<VertexPainterWrapper>> child_wrappers;
                for(const auto& painter_serializer : child_painters_serializers)
                {
                    std::shared_ptr<VertexPainterWrapper> wrapper = std::make_shared<VertexPainterWrapper>();;
                    wrapper->wrap(painter_serializer.get_serialized());
                    child_wrappers.push_back(wrapper);
                }
                set_child_painters(child_wrappers);
            }

        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const std::uint32_t) const
            {
                std::uint32_t unused = 0;
                save_impl<Archive, VertexPainterSerializer>(ar, unused);
            }
        template<class Archive>
        void load(Archive& ar, const std::uint32_t)
            {
                std::uint32_t unused = 0;
                load_impl<Archive, VertexPainterSerializer>(ar, unused);
            }
    };
}

#endif // VERTEX_PAINTER_COMPOSITE_H
