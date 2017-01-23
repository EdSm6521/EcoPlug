class cCircularBuffer
{
  public:
    
	
    int numElements;

    float *elements;
    
	int findex;
    int cindex;
    int lindex;
    
	float maxindex;
    float minindex;
	float avgindex;

	cCircularBuffer(int _numElements = 50)
    {
	  elements = new float[_numElements];
	  numElements = _numElements;
      findex = 0;
      cindex = 0;
      lindex = 0;
      maxindex = 0;
      minindex = 0;
	  avgindex = 0;
    }
    void addElement(float _value)
    {
      elements[cindex] = _value;
      if(cindex == 0)
      {
          maxindex = _value;
          minindex = _value;
      }
       if(maxindex < _value)
       {
         maxindex = _value;
       }
       if(minindex > _value)
       {
         minindex = _value;
       }
      cindex++;
      if (cindex >= numElements)
      {
        cindex = 0;
        findex++;
        if (findex >= numElements)
        {
          findex = 0;
        }
      }
    }

    float getAvg()
    {
      lindex = findex;
      int s = findex;
      float result = 0;
      for (int r = 0; r < numElements; r++)
      {
		  result += elements[s];
        s++;
        if (s >= numElements)
        {
          s = 0;
        }
      }
      result /= (float)numElements;
	  avgindex = result;
      return result;
    }

};
