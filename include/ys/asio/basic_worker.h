/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-07-29
 * \brief  Basic worker class header file.
 */

#ifndef YS_ASIO_BASIC_WORKER_H
#define YS_ASIO_BASIC_WORKER_H

#include <set>
#include <memory>
#include <boost/signals2.hpp>

#include <ys/asio/tcp_connection.h>

namespace ys
{
namespace asio
{

/*!
 * \brief A simple worker class template.
 */
template<class Worker, class Buffer>
class basic_worker
    : public std::enable_shared_from_this<basic_worker<Worker, Buffer>>
{
public:
    /*!
     * Worker target class typedef.
     */
    using worker_type = Worker;

    /*!
     * Basic worker class typedef.
     */
    using basic_worker_type = basic_worker<Worker, Buffer>;

    /*!
     * Worker pointer typedef.
     */
    using ptr = std::shared_ptr<basic_worker<Worker, Buffer>>;

    /*!
     * Connection buffer typedef.
     */
    using buffer_type = Buffer;

    /*!
     * Connection typedef.
     */
    using tcp_conn_type =
        ys::asio::tcp_connection<buffer_type>;

    /*!
     * Connection pointer typedef.
     */
    using tcp_conn_ptr =
        std::shared_ptr<tcp_conn_type>;

    /*!
     * Connection registered signal typedef.
     */
    using on_tcp_conn_reg_type =
        boost::signals2::signal<void (ptr, tcp_conn_ptr)>;

    /*!
     * Connection unregistered signal typedef.
     */
    using on_tcp_conn_unreg_type =
        boost::signals2::signal<void (ptr, tcp_conn_ptr)>;

    /*!
     * Get a reference to the correctly casted worker object.
     * \return
     */
    Worker&
    casted();

    /*!
     * Get a const-reference to the correctly casted worker object.
     * \return
     */
    Worker const&
    casted() const;

    /*!
     * Receive new TCP connection socket.
     * \param socket_ Accepted socket of a new connection.
     */
    void
    on_tcp_socket(boost::asio::ip::tcp::socket socket_);

    /*!
     * Handle shutdown request.
     */
    void
    on_shutdown();

    /*!
     * Set handler for TCP connection registered signal.
     * \param cb
     */
    void
    on_connection_registered(
        typename on_tcp_conn_reg_type::slot_type const& cb);

    /*!
     * Set handler for TCP connection unregistered signal.
     * \param cb
     */
    void
    on_connection_unregistered(
        typename on_tcp_conn_unreg_type::slot_type const& cb);

protected:
    /*!
     * Subscribe for the connection events and add it to the register.
     * \param c Connection pointer.
     */
    tcp_conn_ptr
    register_connection(tcp_conn_ptr c);

    /*!
     * Unsubscribe from the connection events and remove it from the register.
     * \param c Connection pointer.
     */
    void
    unregister_connection(tcp_conn_ptr c);

private:
    /*!
     * A register with TCP connections.
     */
    std::set<tcp_conn_ptr> tcp_connections_;

    /*!
     * Connection registered signal.
     */
    on_tcp_conn_reg_type on_tcp_conn_reg_signal_;

    /*!
     * Connection unregistered signal.
     */
    on_tcp_conn_unreg_type on_tcp_conn_unreg_signal_;

}; // class basic_worker

} // namespace asio
} // namespace ys

#include "basic_worker.tcc"

#endif // YS_ASIO_BASIC_WORKER_H

