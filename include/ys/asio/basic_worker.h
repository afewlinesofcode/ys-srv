/*!
 * \file
 * \author Stanislav Yaranov <stanislav.yaranov@gmail.com>
 * \date   2016-07-29
 * \brief  Basic worker class header file.
 */

#ifndef YS_ASIO_BASIC_WORKER_H
#define YS_ASIO_BASIC_WORKER_H

#include <set>
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
{
public:
    /*!
     * Connection buffer typedef.
     */
    using buffer_type = Buffer;

    /*!
     * Connection typedef.
     */
    using tcp_connection_type =
        ys::asio::tcp_connection<buffer_type>;

    /*!
     * Connection pointer typedef.
     */
    using tcp_connection_ptr =
        std::shared_ptr<tcp_connection_type>;

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

protected:
    /*!
     * Subscribe for the connection events and add it to the register.
     * \param c Connection pointer.
     */
    tcp_connection_ptr
    register_tcp_connection(tcp_connection_ptr c);

    /*!
     * Unsubscribe from the connection events and remove it from the register.
     * \param c Connection pointer.
     */
    void
    unregister_tcp_connection(tcp_connection_ptr c);

private:
    /*!
     * A register with TCP connections.
     */
    std::set<tcp_connection_ptr> tcp_connections_;

}; // class basic_worker

} // namespace asio
} // namespace ys

#include "basic_worker.tcc"

#endif // YS_ASIO_BASIC_WORKER_H

