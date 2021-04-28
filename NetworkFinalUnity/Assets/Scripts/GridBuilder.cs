using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GridBuilder : MonoBehaviour
{
    public int xLength;
    public int zLength;
    public GameObject gridPlatform;
    public GameObject player;


    // Start is called before the first frame update
    void Start()
    {
        BuildGrid();
        
    }

    private void BuildGrid() 
    {
        for (int x = 0; x < xLength; x++) 
        {
            for (int z = 0; z < zLength; z++) 
            {
                Vector3 pos = new Vector3(x + 0.5f, 0, z + 0.5f);
                GameObject gridSquare = Instantiate(gridPlatform, pos, Quaternion.identity);
                gridSquare.transform.SetParent(transform);
            }
        }
        //SpawnPlayer();
    }

    private void SpawnPlayer() 
    {
        Vector3 pos = new Vector3(0.5f, 0.5f, 0.5f);
        GameObject newPlayer = Instantiate(player, pos, Quaternion.identity);
        
    }
}
