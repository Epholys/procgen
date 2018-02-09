#include "LSystemBuffer.h"

namespace procgui
{
    LSystemBuffer::LSystemBuffer(const std::shared_ptr<LSystem>& lsys)
        : lsys_ {lsys}
        , buffer_ {}
        , lock_ {false}
    {
        lsys_::add_callback([this](){sync();});
        
        // Initialize the buffer with the LSystem's rules.
        for (const auto& rule : lsys_::target_->get_rules())
        {
            buffer_.push_back({true, rule.first, rule.second});
        }
    }
    
    LSystem& LSystemBuffer::get_lsys() const
    {
        return *lsys_::target_;
    }

    LSystemBuffer::const_iterator LSystemBuffer::begin()
    {
        return buffer_.begin();
    }

    LSystemBuffer::const_iterator LSystemBuffer::end()
    {
        return buffer_.end();
    }

    void LSystemBuffer::change_predecessor(LSystemBuffer::const_iterator cit, bool valid, predecessor pred)
    {
        Expects(cit != buffer_.end());

        // https://stackoverflow.com/a/10669041/4309005
        auto it = buffer_.erase(cit, cit);
        const successor& succ = std::get<successor>(*cit);
        *it = { valid, pred, succ };

        if (valid)// we added a pred
        {
            lock_ = true;
            lsys_::target_->add_rule(pred, succ);
            lock_ = false;
        }
    }

    void LSystemBuffer::remove_predecessor(LSystemBuffer::const_iterator cit)
    {
        Expects(cit != buffer_.end());

        auto old_pred = std::get<predecessor>(*cit);

        auto it = buffer_.erase(cit, cit);
        const successor& succ = std::get<successor>(*cit);
        *it = { true, '\0', succ };

        bool duplicate = false;
        for (auto same_it = buffer_.begin(); same_it != buffer_.end(); ++same_it)
        {
            auto same_pred = std::get<predecessor>(*same_it);
            if (same_it != it &&
                old_pred == same_pred)
            {
                duplicate = true;
                break;
            }
        }

        if (!duplicate)
        {
            lock_ = true;
            lsys_::target_->remove_rule(old_pred);
            lock_ = false;
        }
        
    }


    void LSystemBuffer::change_successor(const_iterator cit, successor succ)
    {
        Expects(cit != buffer_.end());

        auto it = buffer_.erase(cit, cit);

        auto valid = std::get<validity>(*cit);
        auto pred = std::get<predecessor>(*cit);
        *it = { valid, pred, succ };

        if (valid)
        {
            lock_ = true;
            lsys_::target_->add_rule(pred, succ);
            lock_ = false;
        }
    }

    void LSystemBuffer::erase(const_iterator cit)
    {
        Expects(cit != buffer_.end());
        
        auto valid = std::get<validity>(*cit);
        auto pred = std::get<predecessor>(*cit);
        auto succ = std::get<successor>(*cit);


        if (valid && pred != '\0')
        {
            lock_ = true;
            lsys_::target_->remove_rule(pred);
            lock_ = false;
        }

        for (auto it = buffer_.begin(); it != buffer_.end(); ++it)
        {
            auto same_pred = std::get<predecessor>(*it);
            auto new_succ = std::get<successor>(*it);
            if (it != cit &&
                pred == same_pred)
            {
                if(valid)
                {
                    std::get<validity>(*it) = true;
                }
                lock_ = true;
                lsys_::target_->add_rule(same_pred, new_succ);
                lock_ = false;
                break;
            }
        }

        buffer_.erase(cit);
    }

    void LSystemBuffer::add_rule()
    {
        buffer_.push_back({true, '\0', "" });
    }
    
    
    void LSystemBuffer::sync()
    {
        if(lock_)
        {
            return;
        }
            
    
        const auto& lsys_rules = lsys_::target_->get_rules();

        // gérer les ajouts
        // gérer les suppressions
        // gérer les modifications
        // gérer les \0
        // gérer les non valides
        
        for (const auto& rule : lsys_rules)
        {
            char pred = rule.first;
            auto it = std::find_if(buffer_.begin(), buffer_.end(),
                                   [pred](const auto& tuple)
                                   { return std::get<predecessor>(tuple) == pred &&
                                     std::get<validity>(tuple);                      });
            if (it != buffer_.end())
            {
                std::get<successor>(*it) = rule.second;
            }
            else
            {
                buffer_.push_back({true, rule.first, rule.second});
            }
        }

        for (auto it = buffer_.begin(); it != buffer_.end(); )
        {
            auto valid = std::get<validity>(*it);
            auto pred = std::get<predecessor>(*it);

            if(valid && pred != '\0')
            {
                bool exist = lsys_rules.count(pred) > 0 ? true : false;
                if (!exist)
                {
                    it = buffer_.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }

        // IMPOSSIBLE : lock
        // for (auto it = buffer_.begin(); it != buffer_.end(); ++it)
        // {
        //     if (!std::get<validity>(*it))
        //     {
        //         bool duplicate = false;
        //         for (auto jt = buffer_.begin(); jt != buffer_.end(); ++jt)
        //         {
        //             if (it != jt &&
        //                 std::get<predecessor>(*it) == std::get<predecessor>(*jt))
        //             {
        //                 duplicate = true;
        //                 break;
        //             }
        //         }
        //         if (!duplicate)
        //         {
        //             std::get<validity>(*it) = true;
        //             change_predecessor(it, true, std::get<predecessor>(*it));
        //         }
        //     }
        // }
        
        // lsys_::target_->clear_rules();

        // for (const auto& rule : buffer_)
        // {
        //     char pred = std::get<predecessor>(rule).at(0);
        //     if(std::get<validity>(rule) &&
        //        pred != '\0') // An empty rule is not synchronized.
        //     {
        //         const auto& arr = std::get<successor>(rule);
        //         auto succ = array_to_string(arr);
        //         lsys_::target_->add_rule(pred, succ);
        //     }
        // }
    }
}
