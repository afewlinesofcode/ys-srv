/*!
 * \file
 * \date   2016-06-30
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \brief  A simple server class source file.
 */

#ifndef YS_ASIO_SIMPLE_SERVER_TCC
#define YS_ASIO_SIMPLE_SERVER_TCC

#include "simple_server.h"

#include <mutex>
#include <csignal>

#include <ys/asio/error.h>

namespace ys
{
namespace asio
{

/*!
 * Constructor.
 * \param opts Server options.
 */
template<class Worker>
simple_server<Worker>::simple_server(options const& opts) :
    opts { opts },
    io_service_ { },
    main_service_ { },
    signals_ { main_service_ },
    tcp_acceptor_ { io_service_ }, tcp_socket_ { io_service_ }
{
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
#ifdef SIGQUIT
    signals_.add(SIGQUIT);
#endif

    /*
     * Start waiting for a shutdown signal.
     */
    await_stop();
}

/*!
 * Set up a tcp endpoint for accepting connections.
 * \param address Listen address.
 * \param port Listen port.
 */
template<class Worker>
void
simple_server<Worker>::tcp(std::string const& address, std::string const& port)
{
    using namespace boost::asio::ip;

    tcp::resolver resolver_ { io_service_ };

    tcp::endpoint endpoint_ = *resolver_.resolve( { address, port });

    tcp_acceptor_.open(endpoint_.protocol());
    tcp_acceptor_.set_option(tcp::acceptor::reuse_address { true });
    tcp_acceptor_.bind(endpoint_);
    tcp_acceptor_.listen();

    await_tcp_accept();
}

/*!
 * Start a server.
 */
template<class Worker>
void
simple_server<Worker>::run()
{
    /*!
     * A thread pointer typedef.
     */
    using thread_ptr = std::shared_ptr<std::thread>;

    /*!
     * A container with pointers to processing threads.
     */
    std::vector<thread_ptr> threads;

    /*!
     * Aux mutex for modifying `threads` container.
     */
    std::mutex m;

    for (std::size_t i = 0; i < opts.workers_num; ++i)
    {
        /*!
         * Next worker thread.
         */
        thread_ptr thread
        {
            new std::thread {
                [this, &m]()
                {
                    /*
                     * Create a worker object with a mapping between
                     * current running thread and the worker.
                     */
                    {
                        std::lock_guard<std::mutex> lock(m);

                        workers_.insert(
                        {
                            std::this_thread::get_id(),
                            std::make_shared<Worker>()
                        });
                    }

                    /*
                     * Go working.
                     */
                    io_service_.run();

                    /*
                     * The work is done. The mapping must leave.
                     */
                    {
                        std::lock_guard<std::mutex> lock(m);

                        workers_.erase(std::this_thread::get_id());
                    }
                }
            }
        };

        /*
         * Save the thread just created in the container for future use.
         */
        threads.push_back(thread);
    }

    /*
     * Start the io service of the main thread (wait for shutdown signals).
     */
    main_service_.run();

    /*
     * Join all worker threads.
     */
    for (thread_ptr& p : threads)
    {
        p->join();
    }
}

/*!
 * Wait asynchronously for the next tcp connection and accept it.
 */
template<class Worker>
void
simple_server<Worker>::await_tcp_accept()
{
    tcp_acceptor_.async_accept(tcp_socket_,
                               [this](boost::system::error_code const& ec)
    {
        if (!ec)
        {
            /*
             * No error. Pass the accepted socket to a worker.
             */
            get_worker()->on_tcp_socket(std::move(tcp_socket_));

            /*
             * Continue.
             */
            await_tcp_accept();
        }
        else
        {
            /*
             * Error! The work must finish.
             */
            throw error(ec.message());
        }
    });
}

/*!
 * Wait for the shutdown signal.
 */
template<class Worker>
void
simple_server<Worker>::await_stop()
{
    signals_.async_wait(
        [this](boost::system::error_code const&, int)
    {
        /*
         * Send `close` command to all workers.
         */
        for (auto& it: workers_)
        {
            it.second->on_shutdown();
        }

        /*
         * Stop the main thread IO service.
         */
        main_service_.stop();

        /*
         * Stop the connections processing IO service.
         */
        io_service_.stop();
    });
}

/*!
 * Get a worker object related to a current thread.
 * \return
 */
template<class Worker>
typename simple_server<Worker>::worker_ptr
simple_server<Worker>::get_worker() const
{
    auto it = workers_.find(std::this_thread::get_id());

    if (it == workers_.end())
    {
        throw new std::runtime_error("No suitable worker found");
    }

    return it->second;
}

} /* namespace asio */
} /* namespace ys */

#endif // YS_SIMPLE_SERVER_TCC

