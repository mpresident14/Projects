// Lambdas can only be converted to function pointers if they don't capture

// std::function allows us to use lambdas with closures as objects, but requires
// them to be copyable (and thus executes copy ctor instead of move ctor).

#ifndef MOVABLE_FN_HPP
#define MOVABLE_FN_HPP

#include <type_traits>
#include <iostream>
#include <memory>
#include <utility>

template <typename R, typename ... Args>
class MovableFn {
private:

    // Declare this abstract class so that we can use a non-templated member
    // variable pointer for MovableFn (not allowed to give member variables their 
    // own template). This way, we don't have to specify "decltype(fn)" in
    // the template declaration of a MovableFn.
    struct FnContainerAbstract {
        virtual ~FnContainerAbstract() {}
        virtual R operator()(Args&& ... args) const = 0;
    };

    template<typename F>
    struct FnContainer : FnContainerAbstract {
        FnContainer(F&& f) : f_{ std::forward<F>(f) } {}
        R operator()(Args&& ... args) const
        {
            // The thing to the left of the ellipsis is the pattern to be 
            // expanded. Below expands to:
            // f_(std::forward<T1>(arg1), std::forward<T2>(arg2), etc.)
            return f_(std::forward<Args>(args)...);
        }

        F f_;
    };

    // Shared pointer makes a MovableFn copyable. If did not care about
    // that, we could use a unique pointer instead.
    std::shared_ptr<FnContainerAbstract> fn_;

public:
    // Require function type to adhere to the return and argument types
    // We also make sure that we don't accept a MovableFn as an 
    // argument. Otherwise, copy and move constructors could get swallowed up 
    // by this constructor. 
    template<
        typename F,
        std::enable_if_t<
            std::is_convertible_v<
                R, 
                std::invoke_result_t<F, Args...>>
            && !std::is_same_v<std::decay_t<F>, MovableFn>,
            int> = 0>
    
    MovableFn(F&& f) : fn_{ new FnContainer<F>{std::forward<F>(f)} } {}
    R operator()(Args&& ... args) const
    {
        return (*fn_)(std::forward<Args>(args)...);
    }
};

#endif
