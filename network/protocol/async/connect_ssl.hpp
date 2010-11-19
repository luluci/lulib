#pragma once

namespace lulib {
	namespace network {
		namespace protocol {
			namespace async {

				// io_service��async_read���Z�b�g����
				template<typename Protocol, typename EndpointIterator, typename Success, typename Failure>
				void connect_ssl(Protocol &p, EndpointIterator &it, Success const& success, Failure const& failure) {
					// �v���g�R�������Ă�����I��
					if (!p) return;

					// endpoint�̉������J�n
					p.socket().lowest_layer().async_connect( *it,
						[&](boost::system::error_code const& error) {
							detail::connect_ssl_handle(error, p, ++it, success, failure);
						}
					);

				}

				namespace detail {
					template<typename Protocol, typename EndpointIterator, typename Success, typename Failure>
					void connect_ssl_handle(
						boost::system::error_code const& error,
						Protocol &p,
						EndpointIterator &it,
						Success const& success, Failure const& failure
					) {
						// �v���g�R�������Ă�����I��
						if (!p) return;

						// connect�ɐ�������
						if (!error) {
							// handshake�Ɉڂ�
							handshake(p, success, failure);
						}
						// connect�Ɏ��s����
						// ���Aendpoint_iterator���I�[�ɒB���Ă��Ȃ��Ȃ�A�ċA����
						else if (it != EndpointIterator()) {
							// ���s�����ڑ������
							p.socket().lowest_layer().close();
							// ���g���C
							p.socket().lowest_layer().async_connect( *it,
								[&](boost::system::error_code const& error) {
									connect_ssl_handle(error, p, ++it, success, failure);
								}
							);
						}
						// �I�[�ɒB���Ă�����A���s
						else {
							//p.async_failure();
							failure();
						}
					}

					template<typename Protocol, typename Success, typename Failure>
					void handshake(Protocol &p, Success const& success, Failure const& failure) {
						// �v���g�R�������Ă�����I��
						if (!p) return;

						p.socket().async_handshake( boost::asio::ssl::stream_base::client,
							[&](boost::system::error_code const& error) {
								// handshake�ɐ���
								if (!error) {
									//p.async_success();
									success();
								}
								// ���s
								else {
									//p.async_failure();
									failure();
								}
							}
						);

						return;
					}
				}//namespace detail

			}//namespace async
		}//namespace protocol
	}//namespace network
}//namespace lulib
