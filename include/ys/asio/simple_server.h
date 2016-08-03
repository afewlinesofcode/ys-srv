/*!
 * \file
 * \date   2016-06-30
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \brief  A simple server class header file.
 */

#ifndef YS_ASIO_SIMPLE_SERVER_H
#define YS_ASIO_SIMPLE_SERVER_H

#include <map>
#include <set>
#include <thread>
#include <boost/asio.hpp>
#include <ys/rsrc_set_mt.h>

namespace ys
{
namespace asio
{

/*!
 * \brief   A simple server accepting TCP connections.
 * \details Accepts incoming connections and then passes accepted connections
 *          to workers running in separate threads for processing.
 *          A worker class must:
 *          1) be `DefaultConstructable`;
 *          2) have method `on_tcp_socket(boost::asio::ip::tcp::socket)` to
 *             receive a new connection socket for communication.
 *          3) have method `on_shutdown()` to stop all processing and prepare
 *             for shutdown.
 */
template<class Worker>
class simple_server
{
public:
    /*!
     * Server options.
     */
    struct options
    {
        /*!
         * A number of running processing workers.
         */
        std::size_t workers_num;
    };

    /*!
     * A worker typedef.
     */
    using worker_type = Worker;

    /*!
     * A worker pointer typedef.
     */
    using worker_ptr = std::shared_ptr<Worker>;

    /*!
     * Server opts.
     */
    const options opts;

    /*!
     * Constructor.
     * \param opts Server options.
     */
    simple_server(options const& opts);

    /*!
     * Copy operation is prohibited.
     * \param
     */
    simple_server(simple_server const&) = delete;

    /*!
     * Copy operation is prohibited.
     * \param
     * \return
     */
    simple_server&
    operator=(simple_server const&) = delete;

    /*!
     * Set a tcp endpoint for processing.
     * \param address Listen address.
     * \param port Listen port.
     */
    void
    tcp(std::string const& address, std::string const& port);

    /*!
     * Start a server.
     */
    void
    run();

private:
    /*!
     * TCP connection acceptor pointer typedef.
     */
    using tcp_acceptor_ptr = std::shared_ptr<boost::asio::ip::tcp::acceptor>;

    /*!
     * IO service for processing connections.
     */
    boost::asio::io_service io_service_;

    /*!
     * IO service used by master thread for handling signals.
     */
    boost::asio::io_service main_service_;

    /*!
     * A set of shutdown signals.
     */
    boost::asio::signal_set signals_;

    /*!
     * A set of TCP connection acceptors.
     */
    std::set<tcp_acceptor_ptr> tcp_acceptors_;

    /*!
     * Concurrent set of TCP sockets used by TCP acceptors.
     */
    ys::rsrc_set_mt<boost::asio::ip::tcp::socket> tcp_sockets_;

    /*!
     * A set of processing workers.
     */
    std::map<std::thread::id, worker_ptr> workers_;

    /*!
     * Wait asynchronously for the next tcp connection and accept it.
     * \param tap A pointer to the acceptor.
     */
    void
    await_tcp_accept(tcp_acceptor_ptr tap);

    /*!
     * Wait for the shutdown signal.
     */
    void
    await_stop();

    /*!
     * Get a worker object related to a current thread.
     * \return
     */
    worker_ptr
    get_worker() const;
};

} /* namespace asio */
} /* namespace ys */

#include "simple_server.tcc"

#endif /* YS_ASIO_SIMPLE_SERVER_H */

