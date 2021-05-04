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

	public void GetScores(ref List<ulong> netObjs, ref List<ulong> scores)
	{
		Debug.Log("Getting final scores");
		for (int i = 0; i < xLength * zLength; i++)
		{
			// skip platform with no owner
			if (!platform[i].POwner) continue;

			// get id of owner of current platform
			ulong curr = platform[i].POwner.playerID;

			// get index of net obj id in list
			int index = netObjs.IndexOf(curr);

			if (index == -1) // if not in list yet
			{
				netObjs.Add(curr);  // add to list
				scores.Add(1);      // add score to list
			}
			else // is already in list
			{
				scores[index]++;	// increment corresponding score
			}
		}
	}
}
