#ifndef HTTPSTATUS__H
#define HTTPSTATUS__H

enum HttpStatus {
  OK   = 200,
  CREATED   = 201,
  NO_CONTENT   = 204,
  BAD_REQUEST   = 400,
  NOT_FOUND   = 404,
  SERVER_ERROR   = 500,
  NOT_IMPLEMENTED   = 501
};

#endif
