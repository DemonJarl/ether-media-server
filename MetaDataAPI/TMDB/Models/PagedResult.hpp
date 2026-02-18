#pragma once
#include "Enums/Enums.hpp"
#include "Models/Cacheable.hpp"
#include "Models/Page.hpp"
#include <expected>
#include <json/value.h>
#include <optional>
#include <type_traits>
#include "BaseAPI.hpp"

namespace TMDBAPI::Models 
{
    template<class Model>
    class PagedResult : public BaseAPI
    {
    private:
        using Page = Page<Model>;
        //static_assert(std::is_base_of_v<Cacheable, Model>, "Model should be derived from Cacheable");
        Page currentPage;
        int totalPages{1};
        int totalResults{1};
    public:                                                                                                
        class Iterator
        {
        private:
            PagedResult<Model>* obj_;
        public:
            using value_type = Page;
            using reference = const Page&;
            using pointer = const Page*;
            using iterator_category = std::input_iterator_tag;
            Iterator(PagedResult<Model>* obj=nullptr): obj_ {obj} {}
            reference operator*() const { return obj_->currentPage; }
            Iterator& operator++() { increment(); return *this; }
            Iterator operator++(int) { increment(); return *this; }
            bool operator==(Iterator rhs) const 
            { 
                if (rhs.obj_ == nullptr && obj_ == nullptr)
                    return true;
                if (rhs.obj_ == nullptr || obj_ == nullptr)
                    return false;
                return obj_->currentPage.getPageNum() == rhs.obj_->currentPage.getPageNum() ; 
            }
            bool operator!=(Iterator rhs) const { return !(rhs==*this); }
        protected:
            void increment()
            {
                if (!obj_->hasNext())
                {
                    obj_ = nullptr;
                    return;
                }
                if (!obj_->next())
                {
                    obj_ = nullptr;
                    return;
                }
            }
        };
        PagedResult<Model>(const Json::Value& json, const std::string& url) : currentPage(json)
        {
            this->baseURL = url;

            if (!json.isMember("total_pages") || json["total_pages"].isNull())
                throw std::invalid_argument("");
            totalPages = json["total_pages"].asInt();

            if (!json.isMember("total_results") || json["total_results"].isNull())
                throw std::invalid_argument("");
            totalResults = json["total_results"].asInt();
        }
        Page getPage()
        {
            return currentPage;
        }
        // возвращает итератор на первую страницу 
        Iterator begin()
        {

            // если текущая страница первая то просто возращаем текущий итератор 
            if (currentPage.getPageNum() == 1)
                return Iterator(this);
            
            // если нет то пытаемся получить первую страницу
            std::optional<Page> next = get(1);
            if (next.has_value())
            {
                currentPage = std::move(next.value());
                return Iterator(this);
            }

            return Iterator();
        }
        Iterator end()
        {
            return Iterator();
        }
    protected:
        bool hasNext()
        {
            return currentPage.getPageNum() != totalPages;
        }
        bool next()
        {
            std::optional<Page> next = get(currentPage.getPageNum()+1);
            if (next.has_value())
            {
                currentPage = std::move(next.value());
                return true;
            }
            return false;
        }
        std::optional<Page> get(const int pageNum)
        {
            std::string endpoint = baseURL+"&page="+std::to_string(pageNum);
            std::expected<Page, APIError> Res = getModel<Page>(endpoint);
            if (Res.has_value())
                return std::move(Res.value());
            return {};
        }
    public:
        bool empty()
        {
            return totalResults == 0;
        }
        int getResultCount()
        {
            return totalResults;
        }
        int getPageCount()
        {
            return totalPages;
        }
    };
}