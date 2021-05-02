using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GridBuilder : MonoBehaviour
{
    public int xLength;
    public int zLength;
    public GameObject gridPlatform;
    public GameObject player;
    private Vector3 spawnPos;
    PlatformClaim[] platform;

    // Start is called before the first frame update
    void Start()
    {
        platform = new PlatformClaim[xLength * zLength];
        BuildGrid();
        //TimerController.instance.BeginTimer();
    }

    private void BuildGrid() 
    {
        for (int x = 0; x < xLength; x++) 
        {
            for (int z = 0; z < zLength; z++) 
            {
                Vector3 pos = new Vector3(x * 1.5f, 0, z * 1.5f);//platform 1, 0.2, 1 has x +.05 and z +.05
                if (x == 0 && z == 0) 
                {
                    //SpawnPlayer();
                    //spawnPos = pos;
                    //GameObject newPlayer = Instantiate(player, spawnPos, Quaternion.identity);
                }
                GameObject gridSquare = Instantiate(gridPlatform, pos, Quaternion.identity);
                platform[z * xLength + x] = gridSquare.GetComponent<PlatformClaim>();
                platform[z * xLength + x].platformIndex = z * xLength + x;
                gridSquare.transform.SetParent(transform);
            }
        }
        //SpawnPlayer();
    }

    private void SpawnPlayer() 
    {
        Vector3 pos = new Vector3(0.0f, 1.0f, 0.0f);
        GameObject newPlayer = Instantiate(player, pos, Quaternion.identity);
        
    }

    public void UpdateGrid(PlayerScript player, int index) 
    {
        Debug.Log("UpdateGrid CAlled");
        platform[index].SetOwner(player);
    }
}
