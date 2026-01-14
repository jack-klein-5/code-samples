/**************************************************************************

This sample is from a (completed) personal project I made in Unity. I made
a neural net using PyTorch, but in order to predict user input it needs to
be fed through the weights in a way Unity understands. I didn't really
touch or clean up this code from how it was used in the project, so if
something looks questionable that might be why. Past me probably thought
this wasn't going to be shown to anyone. Fool.

**************************************************************************/


using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;

public class NeuralNet : MonoBehaviour
{
    private float[,,] conv2d1Result = new float[6,25,25];
    private float[,,] maxpool2d1Result = new float[6, 12, 12];
    private float[,,] conv2d2Result = new float[16, 9, 9];
    private float[,,] maxpool2d2Result = new float[16, 4, 4];
    private float[] flatResult = new float[256];
    private float[] fc1Result = new float[120];
    private float[] fc2Result = new float[84];
    private float[] fc3Result = new float[10];

    //private bool once = true;


    private float[,,] testingImageBytes = new float[1, 28, 28];

    // Start is called before the first frame update
    void Start()
    {
        //TestFn();
    }

    private void TestFn()
    {
        /* Load test inputs */
        // NOTE: Have to put test-images.txt back into Assets/Resources to use this
        TextAsset testFile = Resources.Load<TextAsset>("test-images");
        var testFileBytes = testFile.bytes;

        var imageIndex = 0;
        /* i is for cols */
        for (int i = 0; i < 28; ++i)
        {
            /* j is for rows */
            for (int j = 0; j < 28; ++j)
            {
                /* 16 is beginning of file offset */
                var index = 16 + (784 * imageIndex) + (i * 28) + j;
                testingImageBytes[0, i, j] = testFileBytes[index];
            }
        }

        string imageString = "";
        for (int i = 0; i < 28; ++i)
        {
            for (int j = 0; j < 28; ++j)
            {
                var pixel = testingImageBytes[0, j, i];
                if (pixel <= 255.0f && pixel > 127)
                {
                    imageString += "▓";
                }
                else if (pixel <= 127.0f && pixel > 0.0f)
                {
                    imageString += "▒";
                }
                else
                {
                    imageString += "X";
                }
            }

            imageString += "\n";
        }
        Debug.Log(imageString);
    }

    // Update is called once per frame
    void Update()
    {
        // if (once)
        // {
        //     once = false;

        //     var prediction = FeedForward(ref testingImageBytes);
        //     Debug.Log("PREDICTION: " + prediction);
        // }
    }

    private void ClearArrays()
    {
        Array.Clear(conv2d1Result, 0, 3750);
        Array.Clear(maxpool2d1Result, 0, 864);
        Array.Clear(conv2d2Result, 0, 1296);
        Array.Clear(maxpool2d2Result, 0, 256);
        Array.Clear(flatResult, 0, 256);
        Array.Clear(fc1Result, 0, 120);
        Array.Clear(fc2Result, 0, 84);
        Array.Clear(fc3Result, 0, 10);
    }

    public int FeedForward(ref float[,,] input)
    {
        Conv2D(ref input, ref NNParams.conv1Weights, ref conv2d1Result, ref NNParams.conv1Bias, 4);
        Maxpool2D(ref conv2d1Result, ref maxpool2d1Result);
        Conv2D(ref maxpool2d1Result, ref NNParams.conv2Weights, ref conv2d2Result, ref NNParams.conv2Bias, 4);
        Maxpool2D(ref conv2d2Result, ref maxpool2d2Result);
        /* Flatten */
        int i = 0;
        foreach (float value in maxpool2d2Result)
        {
            flatResult[i] = value;
            ++i;
        }
        FC(ref flatResult, ref NNParams.fc1Weights, ref fc1Result, ref NNParams.fc1Bias);
        FC(ref fc1Result, ref NNParams.fc2Weights, ref fc2Result, ref NNParams.fc2Bias);
        FC(ref fc2Result, ref NNParams.fc3Weights, ref fc3Result, ref NNParams.fc3Bias);

        float maxValue = -1;
        int prediction = -1;
        i = 0;
        foreach(float value in fc3Result)
        {
            if (value > maxValue)
            {
                maxValue = value;
                prediction = i;
            }
            ++i;
        }

        ClearArrays();

        return prediction;        
    }

    private float RELU(float input)
    {
        return Math.Max(0, input);
    }

    private void Conv2D(ref float[,,] inputArray, ref float[,,,] paramArray, ref float[,,] resultArray, ref float[] biasArray, int kernelSize)
    {
        var batchSize = paramArray.GetLength(0);
        var channelSize = paramArray.GetLength(1);
        var numResultRows = resultArray.GetLength(1);
        var numResultCols = resultArray.GetLength(2);

        /* For each batch */
        for (int batchIndex = 0; batchIndex < batchSize; ++batchIndex)
        {
            /* For each channel */
            for (int channelIndex = 0; channelIndex < channelSize; ++channelIndex)
            {
                /* Each result square is a row and column */
                for (int resultRowIndex = 0; resultRowIndex < numResultRows; ++resultRowIndex)
                {
                    for (int resultColIndex = 0; resultColIndex < numResultCols; ++resultColIndex)
                    {
                        /* This is one result square, find dot product of superimposed parameter */ 
                        for (int paramRowIndex = 0; paramRowIndex < kernelSize; ++paramRowIndex)
                        {
                            for (int paramColIndex = 0; paramColIndex < kernelSize; ++paramColIndex)
                            {
                                /* Kill me */
                                resultArray[batchIndex, resultRowIndex, resultColIndex] += 
                                    paramArray[batchIndex, channelIndex, paramRowIndex, paramColIndex] * 
                                    inputArray[channelIndex, resultRowIndex + paramRowIndex, resultColIndex + paramColIndex];
                            }
                        }

                        /* Add bias to resultArray value */
                        resultArray[batchIndex, resultRowIndex, resultColIndex] += biasArray[channelIndex];

                        /* RELU result */
                        resultArray[batchIndex, resultRowIndex, resultColIndex] =
                            RELU(resultArray[batchIndex, resultRowIndex, resultColIndex]);
                    }
                }
            }
        }
    }

    private void Maxpool2D(ref float[,,] inputArray, ref float[,,] resultArray)
    {
        var channelSize = inputArray.GetLength(0);
        var numResultRows = resultArray.GetLength(1);
        var numResultCols = resultArray.GetLength(2);

        /* For each channel */
        for (int channelIndex = 0; channelIndex < channelSize; ++channelIndex)
        {
            /* Each result square is a row and column */
            for (int resultRowIndex = 0; resultRowIndex < numResultRows; ++resultRowIndex)
            {
                for (int resultColIndex = 0; resultColIndex < numResultCols; ++resultColIndex)
                {
                    /* This is one result square, find max of kernel */ 
                    /* I'm hardcoding the kernel size to 2, fite me */
                    var upperLeft = inputArray[channelIndex, resultRowIndex * 2, resultColIndex * 2];
                    var lowerLeft = inputArray[channelIndex, resultRowIndex * 2 + 1, resultColIndex * 2];
                    var upperRight = inputArray[channelIndex, resultRowIndex * 2, resultColIndex * 2 + 1];
                    var lowerRight = inputArray[channelIndex, resultRowIndex * 2 + 1, resultColIndex * 2 + 1];

                    /* Find max of corners */
                    var maxUppers = Math.Max(upperLeft, upperRight);
                    var maxLowers = Math.Max(lowerLeft, lowerRight);
                    var max = Math.Max(maxUppers, maxLowers);

                    /* Store in result */
                    resultArray[channelIndex, resultRowIndex, resultColIndex] = max;
                }
            }
        }
    }

    private void FC(ref float[] inputArray, ref float[,] paramArray, ref float[] resultArray, ref float[] biasArray)
    {
        var numResultRows = paramArray.GetLength(0);
        var numCols = paramArray.GetLength(1);
        for (int resultRowIndex = 0; resultRowIndex < numResultRows; ++resultRowIndex)
        {
            for (int colIndex = 0; colIndex < numCols; ++colIndex)
            {
                resultArray[resultRowIndex] += inputArray[colIndex] * paramArray[resultRowIndex, colIndex];
            }

            /* Add bias to result */
            resultArray[resultRowIndex] += biasArray[resultRowIndex];

            /* RELU result */
            resultArray[resultRowIndex] = RELU(resultArray[resultRowIndex]);
        }
    }

}
