#ifndef __DECODER_H__
#define __DECODER_H__

class Decoder {
public:
	virtual void process() = 0;
	virtual void init() = 0;
    virtual void reset() = 0;
    virtual bool needReset() = 0;
};

#endif /* __DECODER_H__ */
