#pragma once

namespace lulib {
	namespace network {
		namespace protocol {
			namespace async {

				// io_service��async_read���Z�b�g����
				template<typename Protocol, typename EndpointIterator>
				void connect_ssl(Protocol &p, EndpointIterator &it) {
					// �v���g�R�������Ă�����I��
					if (!p) return;

					// endpoint�̉������J�n
					p.socket().lowest_layer().async_connect( *it,
						[&](boost::system::error_code const& error) {
							detail::connect_ssl_handle(error, p, ++it);
						}
					);

				}

				namespace detail {
					template<typename Protocol, typename EndpointIterator>
					void connect_ssl_handle(
						boost::system::error_code const& error,
						Protocol &p,
						EndpointIterator &it
					) {
						// �v���g�R�������Ă�����I��
						if (!p) return;

						// connect�ɐ�������
						if (!error) {
							// handshake�Ɉڂ�
							handshake(p);
						}
						// connect�Ɏ��s����
						// ���Aendpoint_iterator���I�[�ɒB���Ă��Ȃ��Ȃ�A�ċA����
						else if (it != EndpointIterator()) {
							// ���s�����ڑ������
							p.socket().lowest_layer().close();
							// ���g���C
							p.socket().lowest_layer().async_connect( *it,
								[&](boost::system::error_code const& error) {
									connect_ssl_handle(error, p, ++it);
								}
							);
						}
						// �I�[�ɒB���Ă�����A���s
						else {
							p.async_failure();
						}
					}

					template<typename Protocol>
					void handshake(Protocol &p) {
						// �v���g�R�������Ă�����I��
						if (!p) return;

						p.socket().async_handshake( boost::asio::ssl::stream_base::client,
							[&](boost::system::error_code const& error) {
								// handshake�ɐ���
								if (!error) {
									p.async_success();
								}
								// ���s
								else {
									p.async_failure();
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
